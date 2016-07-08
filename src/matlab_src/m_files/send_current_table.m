%% Clear workspace and terminal
% clear all;
% clc
% close all;
%% Initialize serial port
%  delete(instrfindall);
% s1=serial('COM11','BaudRate',9600); 
%  s1 = get_serial_port;
% fopen(s1);
% stopasync(s1)
% fclose(s1);
% delete(s1);
% delete(instrfindall);
% clear s1;


% serial communication
s1=serial('COM8','BaudRate',9600); 
%s1 = get_serial_port;
s1.ReadAsyncMode = 'continuous';
%set(s1, 'terminator', 'LF');
fopen(s1);
pause(1)

%% timer
% Start asynchronous reading
readasync(s1);
% pause(1)



% pause(60);

nIter = 100;
%% Initialize catheter and environment
% DefinitionOfCatheterMagneticField_083015 %%
% DefinitionOfCatheterMagneticField_basedon_dataExp0923 %% the parameters are estimated based on the experiment data obtained on 09/23/2015
%DefinitionOfCatheterMagneticField_basedon_dataExp0930 %% the parameters are estimated based on the experiment data obtained on 09/23/2015


%ParameterOptions = struct('W_A',W_A,'F_C',F_C,'Delta_W',Delta_W,'B_0',B_0,...
%    'NumSegments_total',NumSegments_total,'NumSegments',NumSegments,'Delta',Delta,...
%    'NumSegments_tip',NumSegments_tip,'I_cons',I_cons,'C',C,'MuMatrix',MuMatrix,'L_A',L_A,'C_lastSeg',C_lastSeg);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%        Define contact surface coordinate frame       %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                     
% X_1S = [0 0 pi/2];
%X_1S = [0 0 0];

%ParameterOptions.R_1T = rotfun_expo(X_1S);
%ParameterOptions.tiny_thresh = 1e-5;





%% Read currents from a xlsx file on experiment 09/23

% % trajectory along longitude with an offset (totally 27 data point)
%U = xlsread('desired_trajectories_0923.xlsx',4,'A1:AA3');

% trajectory along longitude in the center of the WS (totally 142 data point)
%U = xlsread('desired_trajectories_0923.xlsx',1,'A1:EL3');
% nstepsize = 5;
% P = zeros(3,length(1:nstepsize:size(U, 2)));
% ij = 0;
% for ii = 1:nstepsize:size(U, 2)
%     ij = ij + 1;
%     P(:,ij) = U(:,ii);
% end
% U = P;



% trajectory along latitude_2 (totally 21 data point)
% U = xlsread('desired_trajectories_0923.xlsx',3,'A1:U3');

% % trajectory along latitude_1 (totally 41 data point)
% U = xlsread('desired_trajectories_0923.xlsx',2,'A1:AO3');



%% Read currents from a xlsx file on experiment 09/30
%%% Altitudinal trajectories

% % trajectory 1 (totally 22 data point)
% U = xlsread('desired_trajectories_0930.xlsx',1,'A1:V3');

% % trajectory 2 (totally 22 data point)
% U = xlsread('desired_trajectories_0930.xlsx',2,'A1:V3');

% % trajectory 3 (totally 22 data point)
% U = xlsread('desired_trajectories_0930.xlsx',3,'A1:V3');

% % trajectory 4 (totally 18 data point)
% U = xlsread('desired_trajectories_0930.xlsx',4,'A1:R3');

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% Longitudinal trajectories


% % trajectory 1 (totally 32 data point)
% U = xlsread('desired_longitude_trajectories_0930.xlsx',1,'A1:AF3');

% % trajectory 2 (totally 40 data point)
% U = xlsread('desired_longitude_trajectories_0930.xlsx',2,'A1:AN3');


% % trajectory 3 (totally 40 data point)
% U = xlsread('desired_longitude_trajectories_0930.xlsx',3,'A1:AN3');

% % trajectory 4 (totally 40 data point)
%U = xlsread('desired_longitude_trajectories_0930.xlsx',4,'A1:AN3');

%%
% % Trajectory for coil calibration
% U = 1/1000*xlsread('E:\mri_experiment_2015_12_08\Trajectory_coil_calibration.xlsx',1,'A1:C37');

% % Trajectory for 3d deflection
%U = 1/1000*xlsread('F:\cloud_experiments\MRI_catheter\MRI_images\mri_experiment_2016_01_05\Trajectory_coil_calibrationN.xlsx',1,'A1:C37')';
%U = 1/1000*xlsread('F:\cloud_experiments\MRI_catheter\MRI_images\mri_experiment_2016_01_05\Trajectory_coil_calibrationP.xlsx',1,'A1:C37')';
%U = 1/1000*xlsread('F:\cloud_experiments\MRI_catheter\MRI_images\mri_experiment_2016_01_05\Trajectory_coil_calibrationNP.xlsx',1,'A1:C74')';
% 
% U = U';
%% Manually read currents 

% U = [0 0 0]';
%U = 1/1000*[100 100 100]';
% U = 1/1000*[0,0,0];
% U = 1/1000*[-200 0 0;-200, 0, 200; -200 0 0; -200 0 -200]';
% U = 1/1000*[0 0 0; 0 0 200; 0 0 0; 0 0 -200; 0 0 0]';
% U = 1/1000*[0 0 0; 0 200 0; 0 0 0; 0 -200 0; 0 0 0]';

% U = 1/1000*[0 0 0; -200 0 0; -200, 0, 200; -200 200 0;  -200 0 -200; -200 -200 0; -200 0 0]';
%%
% test 1: keep axial coil at -200mA and then slowly increase currents of
% Yside

% c_Yside = 0:0.005:0.2;
% U = zeros(3,length(c_Yside));
% t = 1:1:length(c_Yside);
% for k = 1:length(c_Yside)
% 
%     U(:,k) = [-0.2 0 -c_Yside(k)]';
%     
%     
% end
% 
% figure(2)
% clf
% plot(t,U(1,:),'r*')
% hold on
% plot(t,U(2,:),'bo')
% plot(t,U(3,:),'gs')
% title('currents for Yside coil with small increments')

%%%%%%%%%%%%%%%%%%%%##################%%%%%%%*****************%%%%%%%%%%%%%%%%%%


%% continuous manipulation for data collection
% 
% iteration = 14;
% perIteration = 20;
% total = iteration*perIteration;
% 
% U_set = zeros(total,3);
% amplitude = -0.2; % change the sign of the amplitude can create an opposite manipulation
% t = 1:1:total;
% for n = 1:total
%     
%     if n <= 1*perIteration
%         U_set(n,1) = -amplitude*sin(n*pi/2/perIteration);
%     else
%         U_set(n,1) = -amplitude;
%     end
%     
%     if n <= 2*perIteration
%         U_set(n,2) = 0;
%     elseif n > 2*perIteration && n <= 3*perIteration
%         U_set(n,2) = amplitude*sin((n-2*perIteration)*pi/2/perIteration);
%     elseif n > 3*perIteration && n <= 4*perIteration
%         U_set(n,2) = amplitude;
%     elseif n > 4*perIteration && n <= 6*perIteration
%         U_set(n,2) = amplitude*sin(pi/2 + (n-4*perIteration)*(-pi/2/perIteration));
%     elseif n >6*perIteration && n <= 7*perIteration
%         U_set(n,2) = -amplitude;
%     elseif n > 7*perIteration && n <=8*perIteration
%         U_set(n,2) = amplitude*sin(3*pi/2 + (n-7*perIteration)*(pi/2/perIteration));
%     else
%         U_set(n,2) = 0;
%     end
%     
%     if n<=8*perIteration
%         U_set(n,3) = 0;
%     elseif n>8*perIteration && n <= 9*perIteration
%         U_set(n,3) = amplitude*sin((n-8*perIteration)*pi/2/perIteration);
%     elseif n>9*perIteration && n<=10*perIteration
%         U_set(n,3) = amplitude;
%     elseif n>10*perIteration && n<=12*perIteration
%         U_set(n,3) = amplitude*sin(pi/2 + (n-10*perIteration)*(-pi/2/perIteration));
%     elseif n>12*perIteration && n <=13*perIteration
%         U_set(n,3) = -amplitude;
%     elseif n>13*perIteration && n<=total
%         U_set(n,3) = amplitude*sin(3*pi/2 + (n-13*perIteration)*(pi/2/perIteration));
%     end
%     
%     
%     
%     
% end
% 
% 
% figure(1)
% clf
% plot(t,U_set(:,1),'r*')
% hold on
% plot(t,U_set(:,2),'bo')
% plot(t,U_set(:,3),'gs')
% title('currents for data collection 01')
% U = U_set';

%%

% for N = 1:1000
%     
%     if N <= 250
%         if N <= 180
%             U = 1/1000*[0 0 180*sin(N*pi/180)];
%         else
%             U = 1/1000*[0 0 180];
%         end
%     end
%     
%     if N > 250 && N < 500
%         if N < 
%             
%         end
%     end
% end
    


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Checking the deflection direction of the new prototype
% max_current_test = 0.16; 
% % 
%  U = [0, 0 , 0]';

% U = [0, 0 , -max_current_test]'; %% check axial coil

% U = [max_current_test, 0 , -max_current_test]'; %% check X side coil
% U = [-max_current_test, 0 , -max_current_test]'; %% check X side coil

% U = [0, max_current_test, -max_current_test]'; %% check Y side coil
% U = [0, -max_current_test, -max_current_test]'; %% check Y side coil


%% experiments for Taoming%%





%% experiments for erdem
% generate_pseudo_random;


%% Currents for Dominique
%% Currents for Dominique, 2016-04-13
maxCurrent=200;
currentIncr=round(maxCurrent/5);
numPos=46;
U=zeros(3,numPos);

%axial
for i=1:5
    U(3,i)=currentIncr*(i-1);
end

U(3,6)=maxCurrent;

%axial
for i=7:16
    U(3,i)=maxCurrent-currentIncr*(i-7);
end

U(3,17)= -maxCurrent;

for i=18:22
    U(3,i)= -maxCurrent+currentIncr*(i-18);
end

U(3,23)=0;

U(3,24:end)=U(3,1:23);

U=U./1000; %A

% xRange=[1:1:numPos];
% figure; plot(xRange,U(1,:), 'r-*', xRange,U(2,:), 'g-*', xRange,U(3,:),'b-*'); 
% legend('side 1','side 2','axial'); xlabel('position number'); ylabel('current, A');
%% Send current to catheter

U = [ 0 , 0 , 0.2; 0.0, 0.1, 0.2; 0.2, 0.1, 0.2; 0.0, 0.0 -0.2]';

tic
fun_reset_arduino(s1); %run this line to stop sending current to the catheter.
toc


%U = [0.0,0.0,-0.12]'; 

pause(0.001);

pauseTime_PRBS = 15;
% tic
fun_send_current_list(U, s1, pauseTime_PRBS);
% toc
% tic
% fun_reset_arduino(s1); %run this line to stop sending current to the catheter.
% toc
fun_reset_arduino(s1); %run this line to stop sending current to the catheter.
pause(2);

fclose(s1);


