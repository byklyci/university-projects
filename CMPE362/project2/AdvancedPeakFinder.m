prompt = 'Enter the folder name in the working directory (e.g. signal.csv): ';
foldername = input(prompt,'s');

allcvsfiles = dir( strcat(foldername,'/**/*.csv') );

for file = allcvsfiles'
    csvdata = csvread(strcat(file.folder, "\", file.name));
    
    % without filter
    [pks,locs,w,p] = findpeaks(csvdata);
    [pks,locs] = findpeaks(csvdata,'MinPeakProminence', ...
        mean(p)+std(p),'MinPeakWidth',mean(w)+std(w));
    
    numOfPeaks = length(locs);
    
    % low pass filter
    for limitFreq=1000:1000:4000
        LPF = dsp.LowpassFilter('PassbandFrequency',limitFreq);
        csvdata2 = LPF(csvdata);
        
        [pks,locs,w,p] = findpeaks(csvdata2);
        [pks,locs] = findpeaks(csvdata2,'MinPeakProminence', ...
            mean(p)+std(p),'MinPeakWidth',mean(w)+std(w));
        
        numOfPeaks(end+1)=length(locs);
        
    end
    
    figure;
    plot(0:length(numOfPeaks)-1,numOfPeaks,'x');
    
    yl = ylim; % Get current limits.
    ylim([0, yl(2)]); % draw y from 0

    xl = xlim; % Get current limits.
    xlim([0, xl(2)]); % draw x from 0
   
    title({'Low Pass Filter -  the number of peaks versus limit frequency'})
    xlabel('limit frequency in kHz')
    ylabel('number of peaks')
end

for file = allcvsfiles'
    csvdata = csvread(strcat(file.folder, "\", file.name));
    
    numOfPeaks = zeros(1,30);
    
    for i=1:30
        mvAvg = dsp.MovingAverage(i);
        csvdata2 = mvAvg(csvdata);
        
        [pks,locs,w,p] = findpeaks(csvdata2);
        [pks,locs] = findpeaks(csvdata2,'MinPeakProminence', ...
            mean(p)+std(p),'MinPeakWidth',mean(w)+std(w));
        
        numOfPeaks(i) = length(locs);
    end
    
    figure;
    plot(numOfPeaks,'x');
    yl = ylim; % Get current limits.
    ylim([0, yl(2)]); % Replace lower limit only with a y of 0.

    xl = xlim; % Get current limits.
    xlim([1, xl(2)]); % Replace lower limit only with a x of 0.
   
    title({'Moving Average Filter - the number of peaks versus changing N (1 to 30)'})
    xlabel('N')
    ylabel('number of peaks')
end