[highY,highfs] = audioread('highFreq.wav');
highLeft=highY(:,1); % Left channel of the sound

[lowY,lowfs] = audioread('lowFreq.wav');
lowLeft=lowY(:,1); % Left channel of the sound


figure; 
subplot(1,2,1);
spectrogram(highLeft,[],[],[],highfs,'yaxis','MinThreshold',-50)

ax=gca;
ylim(ax, [0,3]); % show only between 0 and 3kHz
title({'highFreq.wav - Frequency domain','with -50 min threshold'})

subplot(1,2,2);
plot(highLeft);
title('highFreq.wav - Time domain')


figure;

subplot(1,2,1);
spectrogram(lowLeft,[],[],[],lowfs,'yaxis','MinThreshold',-50)

ax=gca;
ylim(ax, [0,2]); % show only between 0 and 2kHz
title({'lowFreq.wav - Frequency domain','with -50 min threshold'})

subplot(1,2,2);
plot(lowLeft);
title('lowFreq.wav - Time domain')

% for highFreq
threshold = 50;
[s,f,t] = spectrogram(highLeft,[],[],[],highfs,'yaxis');
ms = abs(s); % magnitude of short time fourier transform
ms(ms<threshold)=0; % make values below threshold zero
[~,column] = size(ms); % # of column of stft
freq = []; % largest freq in each column
for col = 1:column
    % index of nonzero value that has greatest index
    index = find(ms(:,col),1,'last');
    % pushes the frequency corresponding to this index to freq array
    freq(end+1) = f(index); 
end
maxfreq = round(max(freq)); % the max frequency among all freq array


% for lowFreq
[s2,f2,t2] = spectrogram(lowLeft,[],[],[],lowfs,'yaxis');
ms2 = abs(s2); % magnitude of short time fourier transform
ms2(ms2<threshold)=0; % make values below threshold zero
[~,column] = size(ms2); % # of column of stft
freq = []; % smallest freq in each column of stft
for col = 1:column
    % if not all values are zero
    if ~isempty(find(ms2(:,col),1))
        % index of nonzero value that has greatest index
        index = find(ms2(:,col),1);
        % pushes the frequency corresponding to this index to freq array
        freq(end+1)= f2(index);  
    end
end
minfreq = round(min(freq)); % the min frequency among all freq array

disp(['Voice spectra: ',num2str(minfreq),' - ',num2str(maxfreq),' Hz']);

