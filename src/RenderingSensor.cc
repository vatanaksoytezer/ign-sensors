/*
 * Copyright (C) 2019 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include <ignition/common/Console.hh>

#include "ignition/rendering/Camera.hh"
#include "ignition/sensors/RenderingSensor.hh"

/// \brief Private data class for RenderingSensor
class ignition::sensors::RenderingSensorPrivate
{
  /// \brief Pointer to the rendering scene
  public: ignition::rendering::ScenePtr scene;

  /// \brief Last time when render update was called.
  public: ignition::common::Time lastRenderTime;

  /// \brief Last time when scene graph was updated.
  public: static ignition::common::Time lastSceneUpdateTime;
};

using namespace ignition;
using namespace sensors;

ignition::common::Time
    ignition::sensors::RenderingSensorPrivate::lastSceneUpdateTime;

//////////////////////////////////////////////////
RenderingSensor::RenderingSensor() :
  dataPtr(new RenderingSensorPrivate)
{
}

//////////////////////////////////////////////////
RenderingSensor::~RenderingSensor()
{
}

/////////////////////////////////////////////////
void RenderingSensor::SetScene(rendering::ScenePtr _scene)
{
  this->dataPtr->scene = _scene;
}

/////////////////////////////////////////////////
rendering::ScenePtr RenderingSensor::Scene() const
{
  return this->dataPtr->scene;
}

/////////////////////////////////////////////////
unsigned int RenderingSensor::SensorCount() const
{
  return this->sensors.size();
}

/////////////////////////////////////////////////
rendering::SensorPtr RenderingSensor::Sensor(unsigned int _idx) const
{
  if (_idx >= this->sensors.size())
  {
    ignerr << "Unable to get rendering sensor. Index out of bounds."
        << std::endl;
    return rendering::SensorPtr();
  }

  return this->sensors[_idx];
}

/////////////////////////////////////////////////
void RenderingSensor::Render(const ignition::common::Time &_now, bool _force)
{
  if (!_force && this->dataPtr->lastRenderTime >= _now)
     return;

  // update scene graph only once per frame for all sensors
  if (_now > this->dataPtr->lastSceneUpdateTime)
  {
    this->dataPtr->scene->PreRender();
    this->dataPtr->lastSceneUpdateTime = _now;
  }

  for (auto rs : this->sensors)
  {
    // rs->PreRender();
    rendering::CameraPtr rc = std::dynamic_pointer_cast<rendering::Camera>(rs);
    if (rc)
    {
      rc->Render();
      rc->PostRender();
//      rc->Update();
    }
  }
  this->dataPtr->lastRenderTime = _now;
}
