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

function [Rects, total_values] = ExtractAndSave(input_filename, extractor)

    addpath(genpath('_EgoSampling_Poleg_CVPR_2015'));

    %% File name
    [video_dir, fname, ~] = fileparts(input_filename);
    if strcmp(video_dir, '') == 1
        video_dir = '.';
    end
    output_filename = [video_dir, '/', fname, '_', extractor,'_extracted'];
    fname_values = [output_filename, '.mat'];

    if (exist(fname_values, 'file') == 2 )
        fprintf('%sVideo file has already been extracted', log_line_prefix);
        return;
    end
    
    %% It Saves a .mat file with detected faces
    switch(extractor)
        case 'face'
            %% Face Detector setup (Works with MATLAB 2015a)
            addpath('_NPDFaceDetector_Liao_PAMI_2016');
            modelFile = 'model_unconstrain.mat';
            load(modelFile, 'npdModel');
        case 'pedestrian'
            %% Pedestrian Detector setup
            %% load precomputed models
            addpath('_PedestrianDetector_Piotrs_MATLAB_PMT');
            inriaModel = load('AcfInriaDetector');
            calModel = load('AcfCaltech+Detector');
            LdcfInriaModel = load('LdcfInriaDetector');
            LdcfCalModel = load('LdcfCaltechDetector');
        case 'coolnet'
        case 'gaze'
            %% Nothing to do
        otherwise
            fprintf('Unknown extractor');
            return;
    end

    %% Reading input and Creating the output
    fprintf('%sReading input video (%s)...\n', log_line_prefix, input_filename);
    reader = VideoReader(input_filename);
    num_frames = reader.NumberOfFrames;
    frame_rate = reader.FrameRate;
    fprintf('%sVideo loaded...\n', log_line_prefix);

    %% Gaussian mask setup ()
    width = reader.width;
    height = reader.height;
    max_size = max(width, height);
    h = fspecial('gaussian', max_size, max_size/4);
    if max_size == width
        h = h(1:max_size, (max_size-height)/2:(max_size+height)/2);
        min_size = height;
    else
        h = h((max_size-width)/2:(max_size+width)/2, 1:max_size);
        min_size = width;
    end

    norm_h = (h - min(min(h))) / (max(max(h)) - min(min(h)));

    switch(extractor)
        case 'face'
            [Rects, total_values] = DetectFaces(norm_h, min_size, reader, num_frames, npdModel);
        case 'pedestrian'
            %% Pedestrian Detector setup
            [Rects, total_values] = DetectPedestrians(norm_h, min_size, reader, num_frames, LdcfInriaModel);
        case 'coolnet'
            cnn_filename = [video_dir, '/', fname, '_coolnet_extracted.csv'];
            [Rects, total_values] = GetCNN(cnn_filename, num_frames);
    end

    save(fname_values, 'total_values', 'Rects', 'frame_rate');
end

function [Rects, total_values] = GetCNN(cnn_filename, num_frames)
    fprintf('%sGetting CNN weights...\n',log_line_prefix);
    cnn_values = load(cnn_filename);
    total_values = zeros(num_frames,1);
    Rects = cell(num_frames,1);
    for i=1:num_frames
        rects(1).col = 1;
        rects(1).row = 1;
        rects(1).width = 1;
        rects(1).height = 1;
	rects(1).gaussianWeight = 1;
	rects(1).faceSizeValue = 1;
        rects(1).score = cnn_values(i);
        total_frame = rects(1).score;
        
        total_values(i) = total_frame;
        Rects{i} = rects;
    end
end

function [Rects, total_values] = DetectFaces(norm_h, min_size, reader, num_frames, npdModel)

    %% Calculating face weights.
    fprintf('%sCalculating face weights...\n',log_line_prefix);
    total_values = zeros(num_frames,1);
    Rects = cell(num_frames,1);
    rect_list = cell(num_frames,1);
    fprintf('%sDone with frame 0/%d...\n', log_line_prefix, num_frames);
    for i=1:num_frames
        frame = read(reader,i);
        
        rect_list{i} = DetectFace(npdModel, frame);
        rects = FilterFaces(rect_list, i, min_size, norm_h);
        detected_faces_size = length(rects);
        
        total_frame = 0;
        
        for j=1:detected_faces_size
            total_frame = total_frame + (rects(j).score * rects(j).gaussianWeight * rects(j).faceSizeValue);
        end
        
        total_values(i) = total_frame;
        Rects{i} = rects;
        
        if mod(i, 100) == 0
            fprintf('%sDone with frame %d/%d...\n', log_line_prefix, i, num_frames);
        end
    end
end

function [Rects, total_values] = DetectPedestrians(norm_h, min_size, reader, num_frames, model)
    %% Calculating pedestrian weights.
    fprintf('%sCalculating pedestrian weights...\n',log_line_prefix);
    total_values = zeros(num_frames,1);
    Rects = cell(num_frames,1);
    rect_list = cell(num_frames,1);
    fprintf('%sDone with frame 0/%d...\n', log_line_prefix, num_frames);
    for i=1:num_frames
        frame = read(reader,i);
        
        rect_list{i} = acfDetect(frame, model.detector);
        rects = FilterPedestrians(rect_list, i, min_size, norm_h);
        numPedestrians = size(rects,2);
        
        total_frame = 0;
        
        for j=1:numPedestrians
            rects(j).faceSizeValue = rects(j).width/min_size;
            total_frame = total_frame + rects(j).score;%(rects(j).score * rects(j).gaussianWeight * rects(j).faceSizeValue);
        end
        
        total_values(i) = total_frame;
        Rects{i} = rects;
        
        if mod(i, 100) == 0
            fprintf('%sDone with frame %d/%d...\n', log_line_prefix, i, num_frames);
        end
    end
end

    %% Detection functions
function rects = FilterFaces(rect_list, frame_idx, min_size, norm_h)
    rects = rect_list{frame_idx};
    detected_faces_size = length(rects);
    j=1;
    while j <= detected_faces_size
        face = 0;
        if rects(j).score > 60
            face = 1;
        elseif rects(j).score > 10
            face = ~isFaceFlash(rects(j), frame_idx, rect_list);
        end
        
        if(face)
            faceRect = [rects(j).col, rects(j).row, rects(j).width, rects(j).height];
            faceRectCenter = floor([faceRect(1)+faceRect(3)/2, faceRect(2)+faceRect(4)/2]);
            rects(j).faceSizeValue = rects(j).width/min_size;
            rects(j).gaussianWeight = norm_h(faceRectCenter(1), faceRectCenter(2));
            j = j + 1;
        else
            rects(j) = [];%Deletes the structure, once it's not a face
            detected_faces_size = detected_faces_size - 1;
        end
    end
end

function rects = FilterPedestrians(rect_list, frame_idx, min_size, norm_h)
    aux = rect_list{frame_idx};
    rects = zeros(0);
    detected_pedestrians_size = size(aux,1);
    j = 1;
    index_aux = 1;
    while index_aux <= detected_pedestrians_size
        pedestrian = 0;
        if aux(index_aux,5) > 100
            pedestrian = 1;
        elseif aux(index_aux,5) > 60
            pedestrian = ~isPedestrianFlash(aux(index_aux,:), frame_idx, rect_list);
        end
        
        if(pedestrian)
            pedestrianRect = aux(index_aux,1:4);
            pedestrianRectCenter = floor([pedestrianRect(1)+pedestrianRect(3)/2, pedestrianRect(2)+pedestrianRect(4)/2]);
            rects(j).row = aux(index_aux,1);
            rects(j).col = aux(index_aux,2);
            rects(j).width = aux(index_aux,3);
            rects(j).height = aux(index_aux,4);
            rects(j).score = aux(index_aux,5);
            rects(j).pedestrianSizeValue = rects(j).width/min_size;
            rects(j).gaussianWeight = norm_h(pedestrianRectCenter(1), pedestrianRectCenter(2));
            j = j + 1;
        end
        index_aux = index_aux + 1;
    end
end

function flash = isFaceFlash(bbox, frame_idx, rect_list, range)

    flash = 1; % TRUE

    if nargin < 4
        range = 3;
    end

    dist_max = 10;

    face_center = face_rect_center(bbox);
    for a = max(1 , frame_idx - range ) : min( length(rect_list), frame_idx + range )
        
        if ~(a == frame_idx)
            aux_rects = rect_list{a};
            for j = 1:length(aux_rects)
                aux_rect = aux_rects(j);
                aux_center = face_rect_center(aux_rect);
                if (center_distance(face_center, aux_center) < dist_max && aux_rect.score > 10)
                    flash = 0; %FALSE
                end
            end
        end
    end
end

function flash = isPedestrianFlash(bbox, frame_idx, rect_list, range)

    flash = 1; % TRUE

    if nargin < 4
        range = 3;
    end

    dist_max = 10;

    pedestrian_center = pedestrian_rect_center(bbox);
    for a = max(1 , frame_idx - range ) : min( length(rect_list), frame_idx + range )
        
        if ~(a == frame_idx)
            aux_rects = rect_list{a};
            for j = 1:size(aux_rects,1)
                aux_rect = aux_rects(j,:);
                aux_center = pedestrian_rect_center(aux_rect);
                if (center_distance(pedestrian_center, aux_center) < dist_max && aux_rect(5) > 60)
                    flash = 0; %FALSE
                end
            end
        end
    end
end

function center = face_rect_center(rect)
    center = [rect.col + rect.width/2, rect.row + rect.height/2];
end

function center = pedestrian_rect_center(rect)
    center = [rect(2) + rect(3)/2, rect(1) + rect(4)/2];
end

function dist = center_distance(center1, center2)
    X = [center1; center2];
    dist = pdist(X,'euclidean');
end
