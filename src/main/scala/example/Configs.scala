package example

import chisel3._
import freechips.rocketchip.config.{Config, Parameters}
import freechips.rocketchip.subsystem.{WithNBigCores, WithNMemoryChannels, WithRV32, WithRoccExample}
import freechips.rocketchip.devices.tilelink.BootROMParams
import freechips.rocketchip.diplomacy.{LazyModule, ValName}
import freechips.rocketchip.tile._
import testchipip._
import icenet._

object ConfigValName {
  implicit val valName = ValName("TestHarness")
}
import ConfigValName._

class WithBootROM extends Config((site, here, up) => {
  case BootROMParams => BootROMParams(
    contentFileName = s"./testchipip/bootrom/bootrom.rv${site(XLen)}.img")
})

class WithExampleTop extends Config((site, here, up) => {
  case BuildTop => (clock: Clock, reset: Bool, p: Parameters) => {
    Module(LazyModule(new ExampleTop()(p)).module)
  }
})

class WithPWM extends Config((site, here, up) => {
  case BuildTop => (clock: Clock, reset: Bool, p: Parameters) =>
    Module(LazyModule(new ExampleTopWithPWM()(p)).module)
})

class WithBlockDeviceModel extends Config((site, here, up) => {
  case BuildTop => (clock: Clock, reset: Bool, p: Parameters) => {
    val top = Module(LazyModule(new ExampleTopWithBlockDevice()(p)).module)
    top.connectBlockDeviceModel()
    top
  }
})

class WithSimBlockDevice extends Config((site, here, up) => {
  case BuildTop => (clock: Clock, reset: Bool, p: Parameters) => {
    val top = Module(LazyModule(new ExampleTopWithBlockDevice()(p)).module)
    top.connectSimBlockDevice(clock, reset)
    top
  }
})

class WithLoopbackNIC extends Config((site, here, up) => {
  case NICKey => NICConfig(inBufPackets = 10)
  case BuildTop => (clock: Clock, reset: Bool, p: Parameters) => {
    val top = Module(LazyModule(new ExampleTopWithIceNIC()(p)).module)
    top.connectNicLoopback()
    top
  }
})

class WithSimNetwork extends Config((site, here, up) => {
  case NICKey => NICConfig(inBufPackets = 10)
  case BuildTop => (clock: Clock, reset: Bool, p: Parameters) => {
    val top = Module(LazyModule(new ExampleTopWithIceNIC()(p)).module)
    top.connectSimNetwork(clock, reset)
    top
  }
})

class BaseExampleConfig extends Config(
  new WithBootROM ++
  new freechips.rocketchip.system.DefaultConfig)

class DefaultExampleConfig extends Config(
  new WithExampleTop ++ new BaseExampleConfig)

class RoccExampleConfig extends Config(
  new WithRoccExample ++ new DefaultExampleConfig)

class PWMConfig extends Config(new WithPWM ++ new BaseExampleConfig)

class SimBlockDeviceConfig extends Config(
  new WithBlockDevice ++ new WithSimBlockDevice ++ new BaseExampleConfig)

class BlockDeviceModelConfig extends Config(
  new WithBlockDevice ++ new WithBlockDeviceModel ++ new BaseExampleConfig)

class LoopbackNICConfig extends Config(
  new WithLoopbackNIC ++ new BaseExampleConfig)

class SimNetworkConfig extends Config(
  new WithSimNetwork ++ new BaseExampleConfig)

class WithTwoTrackers extends WithNBlockDeviceTrackers(2)
class WithFourTrackers extends WithNBlockDeviceTrackers(4)

class WithTwoMemChannels extends WithNMemoryChannels(2)
class WithFourMemChannels extends WithNMemoryChannels(4)

class DualCoreConfig extends Config(
  // Core gets tacked onto existing list
  new WithNBigCores(1) ++ new DefaultExampleConfig)

class RV32ExampleConfig extends Config(
  new WithRV32 ++ new DefaultExampleConfig)


class WithFixedInputStream extends Config((site, here, up) => {
  case BuildTop => (clock: Clock, reset: Bool, p: Parameters) => {
    val top = Module(LazyModule(new ExampleTopWithInputStream()(p)).module)
    top.connectFixedInput(Seq(
      BigInt("1002abcd", 16),
      BigInt("34510204", 16),
      BigInt("10329999", 16),
      BigInt("92101222", 16)))
    top
  }
})

class FixedInputStreamConfig extends Config(
  new WithFixedInputStream ++ new BaseExampleConfig)

class WithSimInputStream extends Config((site, here, up) => {
  case BuildTop => (clock: Clock, reset: Bool, p: Parameters) => {
    val top = Module(LazyModule(new ExampleTopWithInputStream()(p)).module)
    top.connectSimInput(clock, reset)
    top
  }
})

class SimInputStreamConfig extends Config(
  new WithSimInputStream ++new BaseExampleConfig
)

class WithCustomAccelerator extends Config((site, here, up) => {
  case BuildRoCC => Seq((p: Parameters) => LazyModule (
    new AccumulatorExample2(OpcodeSet.custom0 | OpcodeSet.custom1)(p))) //dirotta le custom0 e custom 1 a questo core
})

class WithCustomAccelerator_c0 extends Config((site, here, up) => {
  case BuildRoCC => Seq((p: Parameters) => LazyModule (
    new AccumulatorExample2(OpcodeSet.custom0)(p))) //dirotta le custom0 a questo core
})

class AccumulatorExampleConfig extends Config(
  new WithCustomAccelerator ++ new DefaultExampleConfig)


class WithCustomAcceleratorDummy extends Config((site, here, up) => {
  case BuildRoCC => Seq((p: Parameters) => LazyModule(
    new DummyExample(OpcodeSet.custom0 | OpcodeSet.custom1)(p)))
})

class DummyExampleConfig extends Config(
  new WithCustomAcceleratorDummy ++ new DefaultExampleConfig)

class AccumulatorAndDummyConfig extends Config(
  new WithCustomAccelerator_c0 ++ new DummyExampleConfig
)