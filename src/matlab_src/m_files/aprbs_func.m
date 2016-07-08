% APRBS (Amplitudo modulated Pseudo Random Binary Signal) 
% Program pembangkit sinyal random sebagai data input Quadruple Tank 
 
% Progam ini diadopsi di Kai Dadhe et.al 
 
function [time,y1] = aprbs_func(sampling,length,xmin,xmax,mininterval,maxinterval) 
% function [time,output] = aprbs_func(sampling,length,xmin,xmax,mininterval,maxinterval) 
 
% Fungsi ini berguna utk membangkitkan sinyal Amplitude modulated Pseudo  
% Random Binary Signal (APRBS) utk identifikasi dari suatu sistem dinamik. 
% 
% sampling : waktu sampling 
% length   : panjang sinyal yang akan dibangkitkan 
% xmin     : amplitudo minimal dari sinyal 
% xmax     : amplitudo maximal dari sinyal 
% mininterval : minimal interval yg mungkin terjadi dari perubahan nilai sinyal 
% maxinterval : maximal interval yg mungkin terjadi dari perubahan nilai sinyal 
%
% sampling = 0.2;
% length = 500;
% xmin = 0;
% xmax = 5;
% mininterval = 5;
% maxinterval = 20;
%
if length/sampling<1 
   error('Variable length lebih kecil dari sampling intervall!'); 
end 
 
% Generating (membangkitkan) and plotting (menggambar) APRBS 
y1=aprbs(floor(length/sampling),mininterval,maxinterval,xmin,xmax); 
time=zeros(size(y1)); 
 
for i=1:max(size(time))    
   time(i)=(i-1)*sampling; 
end 
 
ys=[time y1]; 
ys=ys'; 
save .\aprbs_1.mat ys 
stairs(time,y1); 
 
% Pemberian label pada grafik 
xlabel('time(s)'); 
ylabel('flow rate(l/min)'); 
title('Plant Input'); 
grid; 
 
% Local function 1 
function y1 = aprbs(n, mist, mast, miny, maxy, F) 
if nargin<5, 
   error('Input fungsi ini terdiri dari 5 elemen') 
end 
if (mist<1), 
    mist = 1; 
end 
if (mast<mist), 
    mast = mist; 
end 
if (n<1), 
    n = 1; 
end 
if (maxy<miny), 
     temp = maxy; 
     maxy = miny; 
     miny = temp; 
end 
if (nargin<6), 
    F = 0.001; 
elseif (F>=1), 
    F = 0.999; 
end 
 
y0 = prbs(n, mist, mast, F); 
y1 = 0; 
y0old = y0(1); 
value = rand(1)*(maxy-miny)+miny; 
y1(1) = value; 
for i=2:n 
   y0new = y0(i); 
   if (y0new ~= y0old), % State has changed 
      value = rand(1)*(maxy-miny)+miny; 
   end 
   y1(i) = value; 
   y0old = y0new; 
end 
y1 = y1'; 
 
% Local function 2 
 
  function y1 = prbs(n, mist, mast, F ) 
  if (nargin<3), 
    error('Input fungsi ini terdiri dari 3 elemen') 
  end 
  if (mist<1), 
    mist = 1; 
  end 
  if (mast<mist), 
    mast = mist; 
  end 
  if (n<1), 
    n = 1; 
  end 
  if (nargin<4), 
    F = 0.8; 
  end 
  if (F<=0), 
    F = 0.001; 
  elseif (F>1), 
    F = 0.999; 
  end 
  mini  = mist - 0.4999; 
  maxi  = mast + 0.4999; 
  maxu  = 1; 
  minu  = (1 - F) * maxu; 
  deltu = maxu - minu; 
  delta = (maxi - mini) / log(minu); 
  state = sign(rand(1) - 0.5); 
  if (state==0),  
    state = 1; 
  end 
  j = 1; 
  y1 = zeros(n,1); 
  while j <= n 
    t  = round( mini + log(minu + rand(1) * deltu) * delta); 
    state = -state; 
    for k = 1:t, 
      y1(j,1) = state; 
      j      = j + 1; 
      if j>n, break, end 
    end 
 end