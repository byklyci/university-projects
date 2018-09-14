prompt = 'Enter name of the audio file in the working directory (clap.wav): ';
filename = input(prompt,'s');

[y, Fs] = audioread(filename);

Y = fft(y);

threshold = round(1900*length(Y)/Fs);
lowerBound = round(500*length(Y)/Fs);
upperBound = round(9000*length(Y)/Fs);

%mean of frequencies between 500 and 1900 Hz
m1 = mean(abs(Y(lowerBound:threshold)));
%mean of frequencies between 1900 and 9000 Hz
m2 = mean(abs(Y(threshold:upperBound)));

if m1 > 2*m2
    disp('clap sound detected')
else
    disp('snap sound detected')
end

