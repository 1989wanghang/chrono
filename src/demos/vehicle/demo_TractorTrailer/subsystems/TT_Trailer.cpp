// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2014 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Authors: Alessandro Tasora
// =============================================================================
//
// Trailer for articulated vehicle model.
//
// =============================================================================

#include "chrono/assets/ChSphereShape.h"
#include "chrono/assets/ChBoxShape.h"

#include "chrono_models/vehicle/generic/Generic_SolidAxle.h"
#include "chrono_models/vehicle/generic/Generic_MultiLink.h"
#include "chrono_models/vehicle/generic/Generic_Wheel.h"
#include "chrono_models/vehicle/generic/Generic_BrakeSimple.h"

#include "subsystems/TT_Tractor.h"
#include "subsystems/TT_Trailer.h"

using namespace chrono;
using namespace chrono::vehicle;
using namespace chrono::vehicle::generic;

// -----------------------------------------------------------------------------
// Static variables
// -----------------------------------------------------------------------------

const double TT_Trailer::m_chassisMass = 3500;                       // chassis sprung mass
const ChVector<> TT_Trailer::m_chassisCOM(-0.2, 0, 0.8);             // COM location
const ChVector<> TT_Trailer::m_chassisInertia(125.8, 497.4, 531.4);  // chassis inertia (roll,pitch,yaw)
const double TT_Trailer::m_frontaxleMass = 500;                      // frontaxle sprung mass
const ChVector<> TT_Trailer::m_frontaxleREF(2, 0, 0);                // frontaxle ref-chassis relative location
const ChVector<> TT_Trailer::m_frontaxleCOM(0.2, 0, 0);              // frontaxle COM cog-ref relative location
const ChVector<> TT_Trailer::m_frontaxleInertia(50.4, 10.4, 50.4);   // frontaxle inertia (roll,pitch,yaw)

const ChVector<> TT_Trailer::m_frontaxleSphericalJoint(2.0, 0, 0.5);  // joint between frontaxle and trailer chassis
const ChVector<> TT_Trailer::m_frontaxlePullerJoint(3.0, 0, 0.6);     // joint between frontaxle and trailer chassis

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TT_Trailer::TT_Trailer(ChSystem* mysystem, const bool fixed, SuspensionType suspType) : m_suspType(suspType) {
    // -------------------------------------------
    // Create the chassis body
    // -------------------------------------------
    m_chassis = std::shared_ptr<ChBodyAuxRef>(mysystem->NewBodyAuxRef());

    m_chassis->SetIdentifier(100);
    m_chassis->SetName("trailer");
    m_chassis->SetMass(m_chassisMass);
    m_chassis->SetFrame_COG_to_REF(ChFrame<>(m_chassisCOM, ChQuaternion<>(1, 0, 0, 0)));
    m_chassis->SetInertiaXX(m_chassisInertia);
    m_chassis->SetBodyFixed(fixed);

    auto sphere = chrono_types::make_shared<ChSphereShape>();
    sphere->GetSphereGeometry().rad = 0.1;
    sphere->Pos = m_chassisCOM;
    m_chassis->AddAsset(sphere);

    mysystem->Add(m_chassis);

    // -------------------------------------------
    // Create the front steering axle body
    // -------------------------------------------
    m_frontaxle = std::shared_ptr<ChBodyAuxRef>(mysystem->NewBodyAuxRef());

    m_frontaxle->SetIdentifier(101);
    m_frontaxle->SetMass(m_frontaxleMass);
    m_frontaxle->SetFrame_REF_to_abs(ChFrame<>(m_frontaxleREF, ChQuaternion<>(1, 0, 0, 0)));
    m_frontaxle->SetFrame_COG_to_REF(ChFrame<>(m_frontaxleCOM, ChQuaternion<>(1, 0, 0, 0)));
    m_frontaxle->SetInertiaXX(m_frontaxleInertia);
    m_frontaxle->SetBodyFixed(fixed);

    auto sphereB = chrono_types::make_shared<ChSphereShape>();
    sphereB->GetSphereGeometry().rad = 0.1;
    sphereB->Pos = m_frontaxleCOM;
    m_frontaxle->AddAsset(sphereB);

    auto boxB = chrono_types::make_shared<ChBoxShape>();
    boxB->GetBoxGeometry().SetLengths(ChVector<>(0.1, 1.5, 0.1));
    m_frontaxle->AddAsset(boxB);

    mysystem->Add(m_frontaxle);

    // -------------------------------------------
    // Create the frontaxle - trailer chassis joint
    // -------------------------------------------

    m_joint = chrono_types::make_shared<ChLinkLockSpherical>();
    m_joint->Initialize(this->m_chassis, this->m_frontaxle, ChCoordsys<>(this->m_frontaxleSphericalJoint));
    mysystem->Add(m_joint);

    // -------------------------------------------
    // Create the suspension subsystems
    // -------------------------------------------
    m_suspensions.resize(2);

    assert(m_suspType == SuspensionType::SOLID_AXLE || m_suspType == SuspensionType::MULTI_LINK);

    switch (m_suspType) {
        case SuspensionType::SOLID_AXLE:
            m_suspensions[0] = chrono_types::make_shared<Generic_SolidAxle>("FrontSusp");
            m_suspensions[1] = chrono_types::make_shared<Generic_SolidAxle>("RearSusp");
            break;
        case SuspensionType::MULTI_LINK:
            m_suspensions[0] = chrono_types::make_shared<Generic_MultiLink>("FrontSusp");
            m_suspensions[1] = chrono_types::make_shared<Generic_MultiLink>("RearSusp");
            break;
        default:
            break;
    }

    // -----------------
    // Create the wheels
    // -----------------
    m_wheels.resize(4);
    m_wheels[0] = chrono_types::make_shared<Generic_Wheel>("Wheel_FL");
    m_wheels[1] = chrono_types::make_shared<Generic_Wheel>("Wheel_FR");
    m_wheels[2] = chrono_types::make_shared<Generic_Wheel>("Wheel_RL");
    m_wheels[3] = chrono_types::make_shared<Generic_Wheel>("Wheel_RR");

    // -----------------
    // Create the brakes
    // -----------------
    m_brakes.resize(4);
    m_brakes[0] = chrono_types::make_shared<Generic_BrakeSimple>("Brake_FL");
    m_brakes[1] = chrono_types::make_shared<Generic_BrakeSimple>("Brake_FR");
    m_brakes[2] = chrono_types::make_shared<Generic_BrakeSimple>("Brake_RL");
    m_brakes[3] = chrono_types::make_shared<Generic_BrakeSimple>("Brake_RR");
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TT_Trailer::Initialize(const ChCoordsys<>& chassisPos,
                            const bool connect_to_puller,
                            std::shared_ptr<chrono::ChBodyAuxRef> pulling_vehicle) {
    m_chassis->SetFrame_REF_to_abs(ChFrame<>(chassisPos));
    m_frontaxle->SetFrame_REF_to_abs(ChFrame<>(m_frontaxleREF >> chassisPos));

    // -------------------------------------------
    // Create the frontaxle - puller chassis joint
    // -------------------------------------------

    if (connect_to_puller) {
        m_puller = chrono_types::make_shared<ChLinkLockSpherical>();
        m_puller->Initialize(this->m_frontaxle, pulling_vehicle,
                             ChCoordsys<>(this->m_frontaxlePullerJoint) >> chassisPos);
        pulling_vehicle->GetSystem()->Add(m_puller);
    }

    // Initialize the suspension subsystems (specify the suspension subsystems'
    // frames relative to the chassis reference frame).
    m_suspensions[0]->Initialize(m_frontaxle, ChVector<>(0, 0, 0), m_frontaxle, -1);
    m_suspensions[1]->Initialize(m_chassis, ChVector<>(-2, 0, 0), m_chassis, -1);

    // Initialize wheels
    m_wheels[0]->Initialize(m_suspensions[0], LEFT);
    m_wheels[1]->Initialize(m_suspensions[0], RIGHT);
    m_wheels[2]->Initialize(m_suspensions[1], LEFT);
    m_wheels[3]->Initialize(m_suspensions[1], RIGHT);

    // Initialize the four brakes
    m_brakes[0]->Initialize(m_suspensions[0], LEFT);
    m_brakes[1]->Initialize(m_suspensions[0], RIGHT);
    m_brakes[2]->Initialize(m_suspensions[1], LEFT);
    m_brakes[3]->Initialize(m_suspensions[1], RIGHT);
}

// -----------------------------------------------------------------------------
// Set visualization type for the various subsystems
// -----------------------------------------------------------------------------
void TT_Trailer::SetSuspensionVisualizationType(VisualizationType vis) {
    for (size_t i = 0; i < m_suspensions.size(); ++i) {
        m_suspensions[i]->SetVisualizationType(vis);
    }
}

void TT_Trailer::SetWheelVisualizationType(VisualizationType vis) {
    for (size_t i = 0; i < m_wheels.size(); ++i) {
        m_wheels[i]->SetVisualizationType(vis);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
double TT_Trailer::GetSpringForce(int axle, VehicleSide side) const {
    switch (m_suspType) {
        case SuspensionType::SOLID_AXLE:
            return std::static_pointer_cast<ChSolidAxle>(m_suspensions[axle])->GetSpringForce(side);
        case SuspensionType::MULTI_LINK:
            return std::static_pointer_cast<ChMultiLink>(m_suspensions[axle])->GetSpringForce(side);
        default:
            return -1;
    }
}

double TT_Trailer::GetSpringLength(int axle, VehicleSide side) const {
    switch (m_suspType) {
        case SuspensionType::SOLID_AXLE:
            return std::static_pointer_cast<ChSolidAxle>(m_suspensions[axle])->GetSpringLength(side);
        case SuspensionType::MULTI_LINK:
            return std::static_pointer_cast<ChMultiLink>(m_suspensions[axle])->GetSpringLength(side);
        default:
            return -1;
    }
}

double TT_Trailer::GetSpringDeformation(int axle, VehicleSide side) const {
    switch (m_suspType) {
        case SuspensionType::SOLID_AXLE:
            return std::static_pointer_cast<ChSolidAxle>(m_suspensions[axle])->GetSpringDeformation(side);
        case SuspensionType::MULTI_LINK:
            return std::static_pointer_cast<ChMultiLink>(m_suspensions[axle])->GetSpringDeformation(side);
        default:
            return -1;
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
double TT_Trailer::GetShockForce(int axle, VehicleSide side) const {
    switch (m_suspType) {
        case SuspensionType::SOLID_AXLE:
            return std::static_pointer_cast<ChSolidAxle>(m_suspensions[axle])->GetShockForce(side);
        case SuspensionType::MULTI_LINK:
            return std::static_pointer_cast<ChMultiLink>(m_suspensions[axle])->GetShockForce(side);
        default:
            return -1;
    }
}

double TT_Trailer::GetShockLength(int axle, VehicleSide side) const {
    switch (m_suspType) {
        case SuspensionType::SOLID_AXLE:
            return std::static_pointer_cast<ChSolidAxle>(m_suspensions[axle])->GetShockLength(side);
        case SuspensionType::MULTI_LINK:
            return std::static_pointer_cast<ChMultiLink>(m_suspensions[axle])->GetShockLength(side);
        default:
            return -1;
    }
}

double TT_Trailer::GetShockVelocity(int axle, VehicleSide side) const {
    switch (m_suspType) {
        case SuspensionType::SOLID_AXLE:
            return std::static_pointer_cast<ChSolidAxle>(m_suspensions[axle])->GetShockVelocity(side);
        case SuspensionType::MULTI_LINK:
            return std::static_pointer_cast<ChMultiLink>(m_suspensions[axle])->GetShockVelocity(side);
        default:
            return -1;
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TT_Trailer::Synchronize(double time, double braking) {
    // Prepare suspensions for accepting tire forces
    for (auto suspension : m_suspensions) {
        suspension->Synchronize();
    }

    // Apply tire forces to spindle bodies
    for (auto wheel : m_wheels) {
        wheel->Synchronize();
    }

    // Apply braking
    m_brakes[0]->Synchronize(braking);
    m_brakes[1]->Synchronize(braking);
    m_brakes[2]->Synchronize(braking);
    m_brakes[3]->Synchronize(braking);
}

// -----------------------------------------------------------------------------
// Log the hardpoint locations for the front-right and rear-right suspension
// subsystems (display in inches)
// -----------------------------------------------------------------------------
void TT_Trailer::LogHardpointLocations() {
    GetLog().SetNumFormat("%7.3f");

    switch (m_suspType) {
        case SuspensionType::SOLID_AXLE:
            GetLog() << "\n---- FRONT suspension hardpoint locations (RIGHT side)\n";
            std::static_pointer_cast<ChSolidAxle>(m_suspensions[0])->LogHardpointLocations(ChVector<>(0, 0, 0), true);
            GetLog() << "\n---- REAR suspension hardpoint locations (RIGHT side)\n";
            std::static_pointer_cast<ChSolidAxle>(m_suspensions[1])->LogHardpointLocations(ChVector<>(0, 0, 0), true);
            break;
        case SuspensionType::MULTI_LINK:
            GetLog() << "\n---- FRONT suspension hardpoint locations (RIGHT side)\n";
            std::static_pointer_cast<ChMultiLink>(m_suspensions[0])->LogHardpointLocations(ChVector<>(0, 0, 0), true);
            GetLog() << "\n---- REAR suspension hardpoint locations (RIGHT side)\n";
            std::static_pointer_cast<ChMultiLink>(m_suspensions[1])->LogHardpointLocations(ChVector<>(0, 0, 0), true);
            break;
        default:
            break;
    }

    GetLog() << "\n\n";

    GetLog().SetNumFormat("%g");
}

// -----------------------------------------------------------------------------
// Log the spring length, deformation, and force.
// Log the shock length, velocity, and force.
// Log constraint violations of suspension joints.
//
// Lengths are reported in inches, velocities in inches/s, and forces in lbf
// -----------------------------------------------------------------------------
void TT_Trailer::DebugLog(int what) {
    GetLog().SetNumFormat("%10.2f");

    if (what & OUT_SPRINGS) {
        GetLog() << "\n---- Spring (front-left, front-right, rear-left, rear-right)\n";
        GetLog() << "Length [inch]       " << GetSpringLength(0, LEFT) << "  " << GetSpringLength(0, RIGHT) << "  "
                 << GetSpringLength(1, LEFT) << "  " << GetSpringLength(1, RIGHT) << "\n";
        GetLog() << "Deformation [inch]  " << GetSpringDeformation(0, LEFT) << "  " << GetSpringDeformation(0, RIGHT)
                 << "  " << GetSpringDeformation(1, LEFT) << "  " << GetSpringDeformation(1, RIGHT) << "\n";
        GetLog() << "Force [lbf]         " << GetSpringForce(0, LEFT) << "  " << GetSpringForce(0, RIGHT) << "  "
                 << GetSpringForce(1, LEFT) << "  " << GetSpringForce(1, RIGHT) << "\n";
    }

    if (what & OUT_SHOCKS) {
        GetLog() << "\n---- Shock (front-left, front-right, rear-left, rear-right)\n";
        GetLog() << "Length [inch]       " << GetShockLength(0, LEFT) << "  " << GetShockLength(0, RIGHT) << "  "
                 << GetShockLength(1, LEFT) << "  " << GetShockLength(1, RIGHT) << "\n";
        GetLog() << "Velocity [inch/s]   " << GetShockVelocity(0, LEFT) << "  " << GetShockVelocity(0, RIGHT) << "  "
                 << GetShockVelocity(1, LEFT) << "  " << GetShockVelocity(1, RIGHT) << "\n";
        GetLog() << "Force [lbf]         " << GetShockForce(0, LEFT) << "  " << GetShockForce(0, RIGHT) << "  "
                 << GetShockForce(1, LEFT) << "  " << GetShockForce(1, RIGHT) << "\n";
    }

    GetLog().SetNumFormat("%g");
}
