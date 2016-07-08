%% ---erdem--- pseudo random binary sequence
% poly = [1 0 0 0 0 1 1];
% initCond = [0 0 0 0 0 1];
% H = comm.PNSequence('Polynomial',poly,'InitialConditions',initCond);
% bits = length(initCond);
% %
% L = 2^bits - 1;
% period = L;
% numRepeat = 2; %change this number to scale the signal dimension
% dataDuration = L*numRepeat;
% H.SamplesPerFrame =  L*numRepeat;
% disp(L*numRepeat);
% % H.BitPackedOutput = true;
% UP = step(H);
% figure, plot(UP), grid on
% legend('PN Sequence')
% title('PN Sequence')
% xlabel('sample'), ylabel('PN value');
% Amp = 1;
% UP = wgn(dataDuration,1,-25);
dataLoaded = load('prbsP3_fmax8fs20_amp012');
UP = dataLoaded.ynew;
% UP = UP';
dataDuration = length(UP);
U = [zeros(2,dataDuration);UP'];