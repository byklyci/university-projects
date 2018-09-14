prompt = 'Enter the folder name in the working directory (e.g. signal.csv): ';
foldername = input(prompt,'s');

allcvsfiles = dir( strcat(foldername,'/**/*.csv') );

for file = allcvsfiles'
    csvdata = csvread(file.name);
    
    figure;
    [pks,locs,w,p] = findpeaks(csvdata);
    [pks,locs] = findpeaks(csvdata,'MinPeakProminence', ...
        mean(p)+std(p),'MinPeakWidth',mean(w)+std(w));
    plot(csvdata,'-o','MarkerIndices',locs,'MarkerEdgeColor','r')
end
