clear all
close all
wmin = 2*pi*0.1;
wmax = 2*pi*8;

fs = 20;
samp_time = 1/fs;
wnyquist=pi/samp_time;
amp = 0.12;

nticks=ceil((2.8/wmax)/samp_time);
clock_tick=nticks*samp_time;
%
% compute no of shift registers needed
%
ttc=2*pi/wmin/clock_tick;
nshift=ceil(log(ttc+1)/log(2));
%


out=prbs2(wmin,wmax,samp_time,amp);
ytest = [out(1:length(out)/2) out(length(out)/2+1:length(out))];
figure,plot(out)
period = 3;
ynew = repmat(out,period,1);
size(ynew)
figure,plot(ynew)