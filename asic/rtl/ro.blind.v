// technically this is not RTL, but it will be synthesized together with the RTL so it is fine
// this file has been stripped of any information that could identify the standard cell provider

module ro(enable, K, out);
  input enable;
  input [1:0] K;
  output out;

wire stage1;
wire stage2;
wire stage3;
wire stage4;
wire stage5;
wire stage6a;
wire stage6b;
wire stage6c;
wire stage6d;
wire stage7;
wire stage8;
wire stage9;
wire stage10;

ND2D0 nand1 (.A1(enable), .A2(feedback), .ZN(stage1) );

INVD0 inv1 (.I(stage1), .ZN(stage2) );
INVD0 inv2 (.I(stage2), .ZN(stage3) );
INVD0 inv3 (.I(stage3), .ZN(stage4) );
INVD2 inv4 (.I(stage4), .ZN(stage5) );

DEL025D1  del1 (.I(stage5), .Z(stage6a) );
DEL050MD1 del2 (.I(stage5), .Z(stage6b) );
DEL075MD1 del3 (.I(stage5), .Z(stage6c) );
DEL100MD1 del4 (.I(stage5), .Z(stage6d) );

INVD1P5  load1 (.I(stage6a), .ZN() );
INVD1P5  load2 (.I(stage6b), .ZN() );
INVD1P5  load3 (.I(stage6c), .ZN() );
INVD1P5  load4 (.I(stage6d), .ZN() );

MUX4D0 mux1 (.I0(stage6a), .I1(stage6b), .I2(stage6c), .I3(stage6d), .S0(K[1]), .S1(K[0]), .Z(stage7));
DEL050MD1 del5 (.I(stage7), .Z(stage8) );
DEL100MD1 del6 (.I(stage8), .Z(stage9) );
DEL100MD1 del7 (.I(stage9), .Z(stage10) );
DEL100MD1 del8 (.I(stage10), .Z(feedback) );

endmodule    
