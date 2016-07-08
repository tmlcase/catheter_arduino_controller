close all;
clear all;
%y = chirp(t,f0,t1,f1,'method',phi) 
f = 10; %signal sampling rate = 10Hz. 
Fs = f;
ts = 1/f; %0.1 s = sampling time
endTime = 60; %3min
t = 0:ts:endTime-ts;

f0 = 0.1; %initial chirp frequency
t1 = endTime; %initial time
f1 = 2;
y = chirp(t,f0,t1,f1,'linear');
y = y.*0.12;
figure,plot(t,y);
grid on;
L1 = length(y);
figure,spectrogram(y);
Y = fft(y);
%Compute the two-sided spectrum P2. Then compute the single-sided spectrum P1 based on P2 and the even-valued signal length L.

P2 = abs(Y/L1);
P1 = P2(1:L1/2+1);
P1(2:end-1) = 2*P1(2:end-1);
%Define the frequency domain f and plot the single-sided amplitude spectrum P1. The amplitudes are not exactly at 0.7 and 1, as expected, because of the added noise. On average, longer signals produce better frequency approximations.

f = Fs*(0:(L1/2))/L1;
figure,
plot(f,P1)
title('Single-Sided Amplitude Spectrum of X(t)')
xlabel('f (Hz)')
ylabel('|P1(f)|')
