package example

import chisel3._
import chisel3.Bundle
import chisel3.core.{BlackBox, Bool, Clock, Input, IntParam, UInt}
import chisel3.util.Decoupled

class SimInputStream (w:Int) extends BlackBox(Map( "DATA_BITS" -> IntParam(w))){
  val io = IO(new Bundle{
    val clock = Input(Clock())
    val reset = Input(Bool())
    val out = Decoupled(UInt(w.W))
  })
}
