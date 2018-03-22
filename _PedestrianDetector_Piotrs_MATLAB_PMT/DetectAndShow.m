function DetectAndShow(filename)
addpath(genpath('.'));
model = load('LdcfCaltechDetector');
image = imread(filename);
bbs = acfDetect(image, model.detector);
for i=1:size(bbs,1)
    bbox = [bbs(i,1), bbs(i,2), bbs(i,3), bbs(i,4)];
    score = bbs(i,5);
    image = insertObjectAnnotation(image, 'rectangle',...
                            [bbox(1), bbox(2), bbox(3), bbox(4)],...
                            score, 'Color', 'r', 'FontSize', 22);
end

figure;
imshow(image)
end