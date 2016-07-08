clear all;
close all;
poly = [7 6 0];

initCond = [zeros(1,poly(1)-1) 1];
H = comm.PNSequence('Polynomial',poly,'InitialConditions',initCond);
bits = length(initCond);
%%
%compute the period and number of frames
% bits = 6;
L = 2^bits - 1;
Fs = 10;
sampling = 1/Fs;
length1 = L;
disp(L*Fs)
xmin = -0.100;
xmax = 0.100;
% mininterval = 1/Fs;
% maxinterval = 1/Fs;
mininterval = 5;
maxinterval = 15;
[time,y1] = aprbs_func(sampling,length1,xmin,xmax,mininterval,maxinterval) ;
X = y1;
figure,stairs(time,y1); grid on;

period = 1;
ynew = repmat(y1,period,1);
ytest = reshape(ynew,[length(ynew)/period period]);
time_new = 0:1/Fs:1/Fs*(length(ynew)-1);
figure,stairs(time_new,ynew),grid on
%NFFT=2^nextpow2(length(X));
X = ynew;
L1 = length(X);
NFFT = L1;
Y = fft(X,NFFT);
F = ((0:1/NFFT:1-1/NFFT)*Fs).';

magnitudeY = abs(Y);        % Magnitude of the FFT
phaseY = unwrap(angle(Y));  % Phase of the FFT

helperFrequencyAnalysisPlot1(F,magnitudeY,phaseY,NFFT)
%%
Y = fft(X);
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
%%
N = length(X);
fs = Fs;
X_mags = abs(fft(X));
bin_vals = [0:N-1];
fax_Hz = bin_vals*fs/N;
N_2 = ceil(N/2);
figure,plot(fax_Hz(1:N_2), X_mags(1:N_2))
xlabel('Frequency (Hz)')
ylabel('Magnitude');
title('Single-sided Magnitude spectrum (Hertz)');
axis tight

% Number of outputs per frame
% Specify the number of PN sequence samples that the step method outputs as a numeric, 
% positive, integer scalar value. 
% The default is 1. 
% If you set this property to a value of M, 
% then the step method outputs M samples of a PN sequence 
% that has a period of N=2^n?1. The value n represents the degree of the generator polynomial 
% that you specify in the Polynomial property.
% If you set the BitPackedOutput property to false, 
% the samples are bits from the PN sequence. 
% If you set the BitPackedOutput property to true, 
% then the output corresponds to SamplesPerFrame groups of bit-packed samples.