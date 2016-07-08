clear all;
close all;


%% generate a pseudo-random binary sequence as an input to send the catheter
% the outpu will be used in the system identification
%%
% poly = [8 2 0];
% H = comm.PNSequence('Polynomial',poly,'InitialConditions',[0 0 0 0 1 0 0 1]);
% bits = poly(1);
%% 	[9 5 0] [6 5 0] [7 6 0];
poly = [7 6 0];

initCond = [zeros(1,poly(1)-1) 1];
H = comm.PNSequence('Polynomial',poly,'InitialConditions',initCond);
bits = length(initCond);
%%
%compute the period and number of frames
% bits = 6;
L = 2^bits - 1;
period = L;
numRepeat = 2; %change this parameter to increase the number of samples
H.SamplesPerFrame = L*numRepeat;
% H.BitPackedOutput = true;
X = step(H); %this is the pseduo-random sequence, send this to arduino.
X = 0.10.*X;
figure, plot(X), grid on
legend('PN Sequence')
title('PN Sequence')
xlabel('sample'), ylabel('PN value');
disp(length(X))
%%
Fs = 10;
%X = upsample(X,Fs);
time = 0:1/Fs:1/Fs*(length(X)-1);
figure,plot(time,X)
%%
% sampling = 1/Fs;
% length = L;
% xmin = -0.180
% xmax = 0.180
% mininterval = 5;
% maxinterval = 20;
% [time,y1] = aprbs_func(sampling,length1,xmin,xmax,mininterval,maxinterval) ;
% Xnew = y1;

%NFFT=2^nextpow2(length(X));
% amplitude = 0.18;
% X = X.*amplitude;
% zeroInd = find(X==0);
% X(zeroInd) = X(zeroInd) - amplitude;
% figure,plot(time,X)
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