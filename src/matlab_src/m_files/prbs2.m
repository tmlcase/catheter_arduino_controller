% Function to generate a PRBS signal
% Copyright June 4,1996, Babu Joseph, Washington University in St. Louis
%
% out=prbs(wmin,wmax,samp_time,amp)
%
% out = one period of the prbs signal
% amp = amplitute of the signal, signal varies between +amp and -amp
% samp_time= sampling time of signals
% wmin = lower bound of the desired frequency of excitation
% wmax = upper bound for the frequency of excitation
% wmax should be greater than the nyquist frequency given by
% wnyquist=pi/tsample otherwise prbs is not generated
%
% check nyquist frequency
function out=prbs2(wmin,wmax,samp_time,amp);
wnyquist=pi/samp_time;
if wnyquist < wmax ; 
	disp('nyquist frequency');disp(wnyquist);
	disp(' is smaller than wmax. Use a smaller sampling time.');
	disp( ' program aborted');
	out = 0;
	return;
else ;end;
%
% compute clock tick time
nticks=ceil((2.8/wmax)/samp_time);
clock_tick=nticks*samp_time;
%
% compute no of shift registers needed
%
ttc=2*pi/wmin/clock_tick;
nshift=ceil(log(ttc+1)/log(2));
%
%
% Check if nshift is too big
if nshift > 9 ; 
	disp(' sequence length too large! try reduding bandwidth of prbs');
	out=0;
end;

% This generates a pseodo-random bitstream using an LFSR.  It requires
% a polynomial generator function.  
%

% Input arguments
% nshift=No of shift registers
% amp=amplitutde of PRBS
% clock_tick=clock time (should be a multiple of samp_time)
% samp_time=sample time
% Output argumensts
% out= PRBS sequence
% ncycles=no of cycles of the prbs sequence desired
% These are primitive polynomials from Davies(1970);
poly=[	1 1 0 0
	2 1 0 0
	3 1 0 0
	4 1 0 0
	5 2 0 0
	6 1 0 0
	7 1 0 0
	8 7 2 1
	9 4 0 0
	10 3 0 0];

taps=zeros(nshift,1);taps(nshift)=1;
for i=2:4;taps(nshift-poly(nshift,i))=1; end;
states=2^nshift-1;
bin_const=flipud(logspace(0,log10(2^(nshift-1)),nshift)');
out=zeros(states,1); 
reg_val=zeros(states,1) ;
reg=ones(size(taps));
for i=1:(states)
   reg=[rem(sum(taps.*reg),2) ; reg(1:nshift-1)];
   out(i)=reg(nshift) ;
   reg_val(i)=sum(reg.*bin_const);
end ;

len_out=length(out)
out=out';
for i=1:len_out;
if out(i)==0;
out(i)=-amp;
else
out(i)=amp;end;
end
out=ones(nticks,1)*out;
out=reshape(out,1,nticks*(2^nshift-1));
%
% compute and plot the frequency content of the input signal
%
% first we find the fourier transform
z=fft(out);
% compute the frequency axis
%
len=length(z);
wlen=ceil(len/2); % use only first half of transform;
w=(1:wlen)*2*pi/samp_time/len; % compute actual frequencies
zm=abs(z)/len; % compute the magnitude of the fourier transform
% capture the first half of the fourier transform
zm1=zm(2:wlen+1);
% display the signal for viewing
stairs(out); ax=axis;ax(3)=2*ax(3);ax(4)=2*ax(4);axis(ax);
disp( 'the prbs generated is shown in graph');
disp( ' hit any key when done viewing')
pause
% plot the frequency content
disp(' See graph for frequency content of signal');
disp(' hit any key when done viewing');

semilogx(w,zm1);
pause;
disp( ' Summary of PRBS characteristics')
disp(' Length of signal');disp (len);
disp( ' Clock tick time');disp(clock_tick);
disp( ' Duration of prbs');disp(len*samp_time);
out=out';
return
