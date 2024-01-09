/* Driver for wrapper around Dino
 *
 * Author: Niklas Eiling <niklas.eiling@eonerc.rwth-aachen.de>
 * Author: Steffen Vogel <svogel2@eonerc.rwth-aachen.de>
 * SPDX-FileCopyrightText: 2024 Niklas Eiling
 * SPDX-FileCopyrightText: 2020 Institute for Automation of Complex Power Systems, RWTH Aachen University
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <villas/fpga/ips/i2c.hpp>
#include <villas/fpga/node.hpp>

namespace villas {
namespace fpga {
namespace ip {

class Dino : public Node {
public:
  union IoextPorts {
    struct __attribute__((packed)) {
      bool clk_dir : 1;
      bool data_dir : 1;
      bool status_led : 1;
      bool n_we : 1; // write enable (active low)
      bool input_zero : 1;
      bool sat_detect : 1;
      bool gain_lsb : 1;
      bool gain_msb : 1;
    } fields;
    uint8_t raw;

    friend std::ostream &operator<<(std::ostream &stream,
                                    const IoextPorts &ports) {
      return stream << "IoextPorts [clk_dir=" << ports.fields.clk_dir
                    << ", data_dir=" << ports.fields.data_dir
                    << ", status_led=" << ports.fields.status_led
                    << ", n_we=" << ports.fields.n_we
                    << ", input_zero=" << ports.fields.input_zero
                    << ", sat_detect=" << ports.fields.sat_detect
                    << ", gain_lsb=" << ports.fields.gain_lsb
                    << ", gain_msb=" << ports.fields.gain_msb << "]";
    }
    std::string toString() {
      std::stringstream s;
      s << *this;
      return s.str();
    }
  };
  enum Gain { GAIN_1 = 0, GAIN_2 = 1, GAIN_5 = 2, GAIN_10 = 3 };

  Dino();
  virtual ~Dino();
  void setI2c(std::shared_ptr<I2c> i2cdev, uint8_t i2c_channel) {
    this->i2cdev = i2cdev;
    this->i2c_channel = i2c_channel;
  }
  virtual void configureHardware() = 0;

  static constexpr const char *masterPort = "M00_AXIS";
  static constexpr const char *slavePort = "S00_AXIS";

  const StreamVertex &getDefaultSlavePort() const {
    return getSlavePort(slavePort);
  }

  const StreamVertex &getDefaultMasterPort() const {
    return getMasterPort(masterPort);
  }

  IoextPorts getIoextDirectionRegister();
  IoextPorts getIoextOutputRegister();

protected:
  std::shared_ptr<I2c> i2cdev;
  uint8_t i2c_channel;

  IoextPorts getIoextDir();
  IoextPorts getIoextOut();
  void setIoextDir(IoextPorts ports);
  void setIoextOut(IoextPorts ports);
};

class DinoAdc : public Dino {
public:
  DinoAdc();
  virtual ~DinoAdc();
  virtual void configureHardware() override;
};

class DinoDac : public Dino {
public:
  DinoDac();
  virtual ~DinoDac();
  virtual void configureHardware() override;
  void setGain(Gain gain);
  Gain getGain();
};

} // namespace ip
} // namespace fpga
} // namespace villas
