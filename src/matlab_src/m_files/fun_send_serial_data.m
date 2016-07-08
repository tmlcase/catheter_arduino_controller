% send a current (A) to the arduino
% currentAmps - the current data to send as a vector
% sp - the serial sp attached to the arduino
% function [currentValsOut, pause_cnt_ms] = fun_sendSerialData_lin2_RevF(currentAmps, pseqnum, sp)   
function [currentValsOut, pause_cnt_ms] = fun_send_serial_data(currentAmps, sp)   
    
    %package index
    pseqnum = 1;
    
    %convert amps to milliamps
    nchannels = length(currentAmps);
    currentMilliAmps = zeros(nchannels, 1);
    for i = 1:nchannels
        if i==3    
            currentMilliAmps(i) = currentAmps(i) * -1000;
        elseif i ==1
            currentMilliAmps(i) = currentAmps(i) * 1000;
        else
            currentMilliAmps(i) = currentAmps(i) * -1000;
        end
    end
    
    bytesOut = mexPacketBytesFromCurrents(currentMilliAmps, nchannels, pseqnum);
%     bytesOut
%     pause;
    
    while(strcmp(get(sp,'TransferStatus'),'idle')<1)
    end
    
    fwrite(sp, bytesOut, 'uint8', 'async');
    pause_ms = 5;
    max_pause = 2000;
    pause_cnt_ms = 0;
    while(sp.BytesAvailable < 3*nchannels && pause_cnt_ms < max_pause)
%     while(sp.BytesAvailable < (3 * nchannels) && pause_cnt_ms < max_pause)             
        java.lang.Thread.sleep(pause_ms);
        pause_cnt_ms = pause_cnt_ms + pause_ms;
%         fprintf('waiting for serial device... paused %d ms total\n',pause_cnt_ms);
    end
%     fprintf('waiting for serial device... paused %d ms total\n',pause_cnt_ms);
    %bytes = sp.BytesAvailable;
    %bytesIn = fread(sp, bytes);
    
    bytesIn = [];
    while sp.BytesAvailable > 0
        bytesIn = [bytesIn, fread(sp, 1, 'uint8')];
    end
    
%     %print bytes received
%     fprintf('Bytes read:\n');
%     for b = 1:length(bytesIn)
%         fprintf('%s\n',dec2bin(bytesIn(b),8));
%     end
    
    %error checking and unpacking bytes
    currentValsOut = zeros(nchannels,1);
    chan_count = 1;
    cval = 0;
    pindex = 0;
    packet_status = 1;
    first_byte = 1;
    for b = 1:length(bytesIn)
%         fprintf('%s\n',dec2bin(bytesIn(b),8)) %print the byte in decimal format
        if bytesIn(b) >= 128 % first bit is set
            pindex = bitand(bytesIn(b), 15);
            if bytesIn(b) >= 192 % first 2 bits are set
                packet_status = 1;
            else
                packet_status = 0;
            end
            cval = 0;
            first_byte = 1;
        else
            if packet_status == 1
                if first_byte == 1                    
                    cval = cval + bitshift(bitand(bytesIn(b), 63), 6); %first 6 MSBs of 12 bits
                    first_byte = 0;
                else
                    cval = cval + bitand(bytesIn(b), 63); %last 6 LSBs of 12 bits
                    currentValsOut(chan_count) = cval;
                    chan_count = chan_count + 1;
                end
            else
%                 fprintf('error in packet %d: errorcode = %d\n', pindex, bytesIn(b));
                currentValsOut(chan_count) = -1;
                chan_count = chan_count + 1;
            end
        end
    end 
    
%     %print currentValsOut returned to main function
%     fprintf('currents values returned:\n');
%     for i=1:nchannels
%         fprintf('%3.3f ',currentValsOut(i));
%     end
end