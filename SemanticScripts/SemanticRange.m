%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%   This file is part of SemanticFastForward_JVCI.
%
%    SemanticFastForward_JVCI is free software: you can redistribute it and/or modify
%    it under the terms of the GNU General Public License as published by
%    the Free Software Foundation, either version 3 of the License, or
%    (at your option) any later version.
%
%    SemanticFastForward_JVCI is distributed in the hope that it will be useful,
%    but WITHOUT ANY WARRANTY; without even the implied warranty of
%    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%    GNU General Public License for more details.
%
%    You should have received a copy of the GNU General Public License
%    along with SemanticFastForward_JVCI.  If not, see <http://www.gnu.org/licenses/>.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [Threshold_Ranges, num_frames, frame_rate, Semantic_frames_indexes] = SemanticRange(matFileName, varargin)
%% Returns the semantic ranges in video given a .mat file (generated by ExtractAndSave function)

    p = inputParser;

    addRequired(p,'matFileName',@ischar);

    parse(p, matFileName);

    struct_file = load(matFileName);
    Semantic_Value = struct_file.('total_values');
    frame_rate = round(struct_file.('frame_rate'));
    
    addOptional(p, 'MultipleThresholds', false, @islogical);
    addOptional(p, 'Threshold', -1, @isnumeric);
    addOptional(p, 'InputRange', [1 length(Semantic_Value)], @isrow);
    addOptional(p, 'ShowPlot', false, @islogical);
    addOptional(p, 'SavePlot', false, @islogical);
    addOptional(p, 'UseOtsu', true, @islogical);
    parse(p, matFileName, varargin{:});

    Semantic_frames_indexes = [];

    %fprintf('Calculating...');

    max_skip_range = 5 * frame_rate;
    min_semantic_range = max(3*frame_rate, 50);% 50 is a workaround

    range_start = p.Results.InputRange(1,1);
    range_end = p.Results.InputRange(1,2);
    num_frames = range_end - range_start + 1;

    % Construct blurring window.
    sigma = max_skip_range;%250;%max_skip_range/2;max_skip_range*1.5;
    gaussFilter = gausswin(sigma);
    gaussFilter = gaussFilter / sum(gaussFilter); % Normalize.

    Semantic_Value_Gaus = conv(Semantic_Value, gaussFilter);
    Semantic_Value_Gaus = Semantic_Value_Gaus((range_start:range_end)+floor(sigma/2)-1);%Selecting range

    %Semantic_Value_Gaus = Semantic_Value;

    if p.Results.UseOtsu
        max_val = round(max(Semantic_Value_Gaus));
        if p.Results.MultipleThresholds
            stopping_threshold = 0.2 * max_val;% Stopping threshold
            i = 2;
            semantic_gain = max_val;
            thresholds(1) = 0;
            Semantic_Value_Gaus_aux = Semantic_Value_Gaus;
            while(semantic_gain > stopping_threshold)
                thresholds(i) = multithresh(Semantic_Value_Gaus_aux);
                semantic_gain = thresholds(i) - thresholds(i-1);
                Semantic_Value_Gaus_aux = Semantic_Value_Gaus_aux(Semantic_Value_Gaus_aux > thresholds(i));

                i = i + 1;
            end
            thresholds = thresholds(2:end);%Remove the first and last positions (invalid positions)
        elseif p.Results.Threshold > 0 %Arbitrary defined
            thresholds(1) = p.Results.Threshold;
        elseif max_val <= 1 %Values are in range [0,1]
            hist_step = 1000;
            %plot(vector);
            thresholds(1) = multithresh(Semantic_Value_Gaus)/hist_step;
        else
            thresholds(1) = multithresh(Semantic_Value_Gaus);
        end
    else
        thresholds(1) = (max(Semantic_Value_Gaus) + min(Semantic_Value_Gaus))/2;
    end

    if thresholds == 0
        num_thresholds = 0;
    else
        num_thresholds = length(thresholds);
    end

    Threshold_Ranges = cell(num_thresholds, 1);

    i = num_thresholds;
    Masked_by_threshold = zeros(range_end-range_start+1, 1);
    Semantic_Value = Semantic_Value(range_start:range_end);%Focusing the video section
    while i > 0
        Ranges = [];
        if i < num_thresholds
            Masked_by_threshold((Semantic_Value >= thresholds(i)) & ~Masked_by_threshold) = i;% Tagging the frames with labels
        else
            Masked_by_threshold(Semantic_Value > thresholds(i)) = i;% Tagging the frames with labels
        end

        Semantic_frames_indexes = find(Masked_by_threshold == i);
        if ~isempty(Semantic_frames_indexes)
            Semantic_skips = Semantic_frames_indexes(2:end) - Semantic_frames_indexes(1:end-1);
            gaps = find(Semantic_skips > 1);
            pre_indexes = Semantic_frames_indexes(gaps);
            pos_indexes = Semantic_frames_indexes(gaps+1);

            for j=1:length(pre_indexes)
                % If we find any non-zero elements this gap is overlapping others
                if (sum(Masked_by_threshold(pre_indexes(j)+1:pos_indexes(j)-1)))
                    Semantic_skips(gaps(j)) = Inf;% Set to the max value to force a Large skip
                end
            end

            Large_skips = [0; find(Semantic_skips > max_skip_range); length(Semantic_frames_indexes)];
            Ranges = zeros(2, size(Large_skips,1)-1);
            for j=1:size(Large_skips,1)-1
                range_min = Semantic_frames_indexes(Large_skips(j)+1);
                range_max = Semantic_frames_indexes(Large_skips(j+1));
                Ranges(:,j) = [range_min; range_max];
            end

            %% Filter ranges with less than 'min_semantic_range' frames.
            Masked_by_threshold(Masked_by_threshold == i) = 0; % Cleanning up the mess before setting the correct labels
            for k=size(Ranges,2):-1:1
                range_min = Ranges(1,k);
                range_max = Ranges(2,k);

                if range_max - range_min < min_semantic_range
                    Ranges(:,k) = [];
                else
                    Masked_by_threshold(range_min:range_max) = i;% Labeling the semantic regions with id
                end
            end
        end

        %Ranges
        if isempty(Ranges)
            Threshold_Ranges = [Threshold_Ranges(1:i-1); Threshold_Ranges(i+1:end)];
        else
            Threshold_Ranges{i} = Ranges;
        end

        %Threshold_Ranges{i} = Ranges;
        i = i -1;
    end

    if p.Results.ShowPlot || p.Results.SavePlot
        figure;
        grid on;
        t = title('Semantic Range Plot','FontSize', 25);
        xl = xlabel('Frame Number','FontSize', 25);
        yl = ylabel('Semantic Score','FontSize', 25);
        hold on;
        plot(Semantic_Value_Gaus, 'LineWidth',2);
        for b=1:size(Threshold_Ranges,1)
            Ranges = Threshold_Ranges{b};
            color = [1-1/b 1/b 1/b];
            for a=1:size(Ranges,2)
                rectangle('Position', [Ranges(1,a), 0, (Ranges(2,a) - Ranges(1,a)), max(Semantic_Value_Gaus)], 'EdgeColor', color, 'LineWidth', 3);
            end
        end

        for i=1:length(thresholds)
            plot([0,length(Semantic_Value_Gaus)],[thresholds(i), thresholds(i)],'Color','g','LineWidth',4)
        end
        hold off

        f1 = gcf;
        f1.PaperPositionMode = 'auto';
        set(f1, 'Position', get(0,'Screensize'));% Maximize figure.

        if p.Results.SavePlot
            [dir, fname, ~] = fileparts(matFileName);
            matlab_version = strsplit(version);
            if strcmp(matlab_version(2), '(R2015a)') || strcmp(matlab_version(2), '(R2016a)')
                saveas(f1, [dir, '/', fname], 'png');
            else
                set(gcf, 'Position', get(0,'Screensize')); % Maximize figure.
                saveas(gcf, [dir, '/', fname], 'png');
            end
            set(gcf, 'Visible', 'off');
        end
    end

    %% Add Start Index Offset
    if(num_thresholds > 0)
        for i=1:length(Threshold_Ranges)
            Threshold_Ranges{i} = Threshold_Ranges{i} + range_start-1;
        end
    end
end