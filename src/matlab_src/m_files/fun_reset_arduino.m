function fun_reset_arduino(s1)
    nchannels = 3;
    currentMilliAmps = zeros(nchannels,1);
    bytesOut = mexPacketBytesFromCurrents(currentMilliAmps, nchannels, 1);
    
    while(strcmp(get(s1,'TransferStatus'),'idle')<1)
    end
    
    fwrite(s1, bytesOut, 'uint8', 'async');
    pause_ms = 5;
    max_pause = 2000;
    pause_cnt_ms = 0;
    while(s1.BytesAvailable < 3*nchannels && pause_cnt_ms < max_pause)
%     while(sp.BytesAvailable < (3 * nchannels) && pause_cnt_ms < max_pause)             
        java.lang.Thread.sleep(pause_ms);
        pause_cnt_ms = pause_cnt_ms + pause_ms;
%         fprintf('waiting for serial device... paused %d ms total\n',pause_cnt_ms);
    end
%     fprintf('waiting for serial device... paused %d ms total\n',pause_cnt_ms);
    %bytes = sp.BytesAvailable;
    %bytesIn = fread(sp, bytes);
    
    bytesIn = [];
    while s1.BytesAvailable > 0
        bytesIn = [bytesIn, fread(s1, 1, 'uint8')];
    end
end

