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
// Authors: Radu Serban
// =============================================================================
//
// Rear of the articulated vehicle model. This base class has a structure similar
// to that of a ChWheeledVehicle.
//
// =============================================================================

#ifndef ARTICULATED_REAR_H
#define ARTICULATED_REAR_H

#include "chrono/physics/ChLinkMotorRotationAngle.h"
#include "chrono_vehicle/wheeled_vehicle/ChWheeledVehicle.h"
#include "subsystems/Articulated_Front.h"

class Articulated_Rear {
  public:
    Articulated_Rear(std::shared_ptr<Articulated_Chassis> front);

    int GetNumberAxles() const { return 1; }

    void Initialize();

    void Synchronize(double time, double steering, double braking);

    void SetSuspensionVisualizationType(chrono::vehicle::VisualizationType vis);
    void SetWheelVisualizationType(chrono::vehicle::VisualizationType vis);

    /// Get all wheels.
    const chrono::vehicle::ChWheelList& GetWheels() const { return m_wheels; }

    /// Get the global location of the specified spindle.
    const chrono::ChVector<>& GetSpindlePos(chrono::vehicle::VehicleSide side) const;

    /// Get the orientation of the specified spindle.
    const chrono::ChQuaternion<>& GetSpindleRot(chrono::vehicle::VehicleSide side) const;

    /// Get the linear velocity of the specified spindle.
    const chrono::ChVector<>& GetSpindleLinVel(chrono::vehicle::VehicleSide side) const;

    /// Get the angular velocity of the specified spindle.
    chrono::ChVector<> GetSpindleAngVel(chrono::vehicle::VehicleSide side) const;

  private:
    std::shared_ptr<Articulated_Chassis> m_front;  ///< handle to front side

    std::shared_ptr<chrono::ChBodyAuxRef> m_chassis;              ///< handle to the chassis body
    std::shared_ptr<chrono::vehicle::ChSuspension> m_suspension;  ///< handle to the suspension subsystem
    chrono::vehicle::ChWheelList m_wheels;                        ///< list of handles to wheel subsystems
    chrono::vehicle::ChBrakeList m_brakes;                        ///< list of handles to brake subsystems

    std::shared_ptr<chrono::ChLinkMotorRotationAngle> m_motor;

    // Chassis mass properties
    static const double m_chassisMass;
    static const chrono::ChVector<> m_chassisCOM;
    static const chrono::ChVector<> m_chassisInertia;

    static const chrono::ChVector<> m_offset;
};

#endif
