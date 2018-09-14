close all

[y1, Fs] = audioread('mike.wav');
[y2, Fs] = audioread('street.wav');

y3 = y1 + y2; %combined sound

NFFT = length(y3);
Y = fft(y3,NFFT);

Y1 = fft(y1,NFFT);
Y2 = fft(y2,NFFT);

lowerBoundFreq = 95;
upperBoundFreq = 3000;
lbfAdjusted = round( length(Y)*lowerBoundFreq/Fs );
ubfAdjusted = round( length(Y)*upperBoundFreq/Fs );
middleAdjusted = length(Y)/2;
endAdjusted = length(Y);

Ycleared = Y;

Ycleared(1:lbfAdjusted) = 0; % delete frequencies between 0 - 95 Hz
Ycleared(ubfAdjusted:middleAdjusted)=0; % delete frequencies between 3 - 11 KHz

Ycleared(endAdjusted-lbfAdjusted:endAdjusted) = 0; % delete frequencies between -95 and 0 Hz
Ycleared(middleAdjusted:endAdjusted-ubfAdjusted) = 0; % delete frequencies between -11 and -3 KHz

y3cleared = ifft(Ycleared,NFFT,'symmetric');
 
sound(y3cleared,Fs)

f = (0:length(Y)-1)*Fs/length(Y);

figure;
subplot(3,1,1);
plot(f,abs(Y1))
ax=gca;
ylim(ax, [0,300])
title('mike.wav - freq. domain')

subplot(3,1,2);
plot(f,abs(Y2))
title('street.wav - freq. domain')

subplot(3,1,3);
plot(f,abs(Y))
ax=gca;
ylim(ax, [0,300])
title('mike+street.wav - freq. domain')

figure;
subplot(3,1,1);
plot(y1)
title('mike.wav - time domain')

subplot(3,1,2);
plot(y2)
title('street.wav - time domain')

subplot(3,1,3);
plot(y3)
title('mike+street.wav - time domain')

figure;
subplot(2,1,1);
plot(f,abs(Y1))
ax=gca;
ylim(ax, [0,300])
title('mike.wav - freq. domain')

subplot(2,1,2);
plot(f,abs(Ycleared))
title('filtered mike+street.wav - freq. domain')

figure;
subplot(2,1,1);
plot(y1)
title('mike.wav - time domain')

subplot(2,1,2);
plot(y3cleared)
title('filtered mike+street.wav - time domain')

snr = 10*log10(sum(y1.^2) / sum((y3cleared-y1).^2));
disp(['SNR: ',num2str(snr)]);