K=0.1;
a=0.5;
N=1;

[y, Fs] = audioread('mike.wav');
yDelayed = [zeros(Fs*K, 1); y]; % 0.1 sec delay
yCombined = yDelayed(1:length(y)) + y;

snrForChangingA = [];
for a=0:0.1:1
	filtered = nTap(yCombined,0.1,a,2,Fs);
	snr = 10*log10(sum(y.^2) / sum((filtered-y).^2));
	snrForChangingA(end+1) = snr;
end

snrForChangingN = [];
for N=1:1:50
	filtered = nTap(yCombined,0.1,0.5,N,Fs);
	snr = 10*log10(sum(y.^2) / sum((filtered-y).^2));
	snrForChangingN(end+1) = snr;
end

snrForChangingK = [];
for K=0.1:0.1:0.4
	filtered = nTap(yCombined,K,0.5,2,Fs);
	snr = 10*log10(sum(y.^2) / sum((filtered-y).^2));
	snrForChangingK(end+1) = snr;
end

figure;
subplot(2,2,1);
plot(0:0.1:1, snrForChangingA);
title('SNR for changing A, K=0.1s, N=2')

subplot(2,2,2);
plot(1:50, snrForChangingN);
title('SNR for changing N, K=0.1s, A=0.5')

subplot(2,2,3);
plot(0.1:0.1:0.4, snrForChangingK);
title('SNR for changing K, A=0.5, N=2')

function result = nTap(signal,K,a,N,Fs)
result=signal;
for i=1:N
	delayedInput = [zeros(round(Fs*K*i), 1); signal]; % K second delay, i times
	delayedInput = delayedInput.*((-a)^i);
	result = result + delayedInput(1:length(signal));
end
end