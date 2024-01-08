/* Driver for wrapper around standard Xilinx Aurora (xilinx.com:ip:aurora_8b10b)
 *
 * Author: Steffen Vogel <post@steffenvogel.de>
 * SPDX-FileCopyrightText: 2017 Institute for Automation of Complex Power Systems, RWTH Aachen University
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstdint>

#include <villas/utils.hpp>

#include <villas/fpga/ips/dino.hpp>

using namespace villas::fpga::ip;

Dino::Dino() : Node(), i2cdev(nullptr), i2c_channel(0) {}

Dino::~Dino() {}

void Dino::setIoextDir(IoextPorts ports) {
  if (i2cdev == nullptr) {
    throw RuntimeError("I2C device not set");
  }
  std::vector<u8> data = {I2C_IOEXT_REG_DIR, ports.raw};
  i2cdev->write(I2C_IOEXT_ADDR, data);
}

void Dino::setIoextOut(IoextPorts ports) {
  if (i2cdev == nullptr) {
    throw RuntimeError("I2C device not set");
  }
  std::vector<u8> data = {I2C_IOEXT_REG_OUT, ports.raw};
  i2cdev->write(I2C_IOEXT_ADDR, data);
}

Dino::IoextPorts Dino::getIoextDir() {
  if (i2cdev == nullptr) {
    throw RuntimeError("I2C device not set");
  }
  std::vector<u8> data = {I2C_IOEXT_REG_DIR};
  i2cdev->write(I2C_IOEXT_ADDR, data);
  i2cdev->read(I2C_IOEXT_ADDR, data, 1);
  IoextPorts ports;
  ports.raw = data[0];
  return ports;
}

Dino::IoextPorts Dino::getIoextOut() {
  if (i2cdev == nullptr) {
    throw RuntimeError("I2C device not set");
  }
  std::vector<u8> data = {I2C_IOEXT_REG_OUT};
  i2cdev->write(I2C_IOEXT_ADDR, data);
  i2cdev->read(I2C_IOEXT_ADDR, data, 1);
  IoextPorts ports;
  ports.raw = data[0];
  return ports;
}

DinoAdc::DinoAdc() : Dino() {}

DinoAdc::~DinoAdc() {}

void DinoAdc::configureHardware() {
  if (i2cdev == nullptr) {
    throw RuntimeError("I2C device not set");
  }
  i2cdev->getSwitch().setChannel(i2c_channel);
  IoextPorts ioext = {.raw = 0};
  ioext.fields.sat_detect = true;
  setIoextDir(ioext);
  ioext.raw = 0;
  ioext.fields.data_dir = true;
  ioext.fields.status_led = true;
  ioext.fields.n_we = true;
  ioext.fields.input_zero = true;
  setIoextOut(ioext);
  ioext.fields.n_we = false;
  setIoextOut(ioext);
}

DinoDac::DinoDac() : Dino() {}

DinoDac::~DinoDac() {}

void DinoDac::configureHardware() {
  if (i2cdev == nullptr) {
    throw RuntimeError("I2C device not set");
  }
  i2cdev->getSwitch().setChannel(i2c_channel);
  IoextPorts ioext = {.raw = 0};
  setIoextDir(ioext);
  ioext.fields.status_led = true;
  setIoextOut(ioext);
  setGain(GAIN_1);
}

void DinoDac::setGain(Gain gain) {
  if (i2cdev == nullptr) {
    throw RuntimeError("I2C device not set");
  }
  i2cdev->getSwitch().setChannel(i2c_channel);
  IoextPorts ioext = getIoextOut();
  ioext.fields.gain_lsb = gain & 0x1;
  ioext.fields.gain_msb = gain & 0x2;
  setIoextOut(ioext);
}

Dino::Gain DinoDac::getGain() {
  if (i2cdev == nullptr) {
    throw RuntimeError("I2C device not set");
  }
  i2cdev->getSwitch().setChannel(i2c_channel);
  IoextPorts ioext = getIoextOut();
  return static_cast<Gain>((ioext.fields.gain_msb << 1) |
                           ioext.fields.gain_lsb);
}

static char n_adc[] = "DINO ADC";
static char d_adc[] = "DINO analog to digital converter";
static char v_adc[] = "xilinx.com:module_ref:dinoif_fast:";
static NodePlugin<DinoAdc, n_adc, d_adc, v_adc> f_adc;

static char n_dac[] = "DINO DAC";
static char d_dac[] = "DINO digital to analog converter";
static char v_dac[] = "xilinx.com:module_ref:dinoif_dac:";
static NodePlugin<DinoDac, n_dac, d_dac, v_dac> f_dac;
