function fun_generate_play_file(playString, currentList, delay)
%
% fun_generate_play_file(playString,currentList, delay)
%
% Description:
% This function generates play file from a current list and delay.
%
% Inputs:
% playString is the name of the output play file.
% currentList is a list of current. 
%  Each current vector is a column vector.
%  Current is in A.
% delay is the time interval between two current vectors.
%  Delay time is in ms.
%

[chan curr] = size(currentList);
fprintf('The output current is %d channels by %d commands\n',chan, curr);
if( length(delay) == 1)
    fprintf('One delay (%f) is used for all of the commands\n',delay);
    delayOut = delay*ones(1,curr);
else
    fprintf('A vector of delays exists');
   if (length(delay) ~= curr)
       fprintf('The vector of delays is mismatched. using the first delay of %f\n',delay(1));
       delayOut = delay(1)*ones(1,curr);
   else
       delayOut = delay;
   end
   
end

% open the file:
fileHandle = fopen(playString,'w');

for ( i = 1:curr)
    chanList = [];
   for ( j = 1:chan)
       if ( i > 1)
          if( currentList(j, i) ==  currentList(j, i-1))
          else
             chanList = [chanList j];
          end
       else
           chanList = [chanList j];
       end
   end
   for (k = chanList)
       fprintf(fileHandle,'%d, %f, ',k,currentList(k,i)*1000);      
       if( k == chanList(end))
          fprintf(fileHandle,'%f \r\n',delayOut(i));  %windows style line feeds 
       else
          fprintf(fileHandle,'0 \r\n');  %windows style line feeds
       end
   end
end

fclose(fileHandle);
