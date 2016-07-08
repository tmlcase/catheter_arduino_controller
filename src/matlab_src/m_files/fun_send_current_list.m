function fun_send_current_list(U, s1, pTime)
  


% U is implicitly m x n where m is the number of channels
% n is the number of steps.
for n = 1:1:size(U, 2)
    tic
    current = U(:, n);

    arduinoRead = fun_send_serial_data(current, s1);
    pause(pTime);
    toc
    
    %disp(arduinoRead);
%     pause;
end
    
end