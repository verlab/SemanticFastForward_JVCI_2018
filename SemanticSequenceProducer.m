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

%% Class Name: SemanticSequenceProducer
% 
% This class is an adaptation of the SequenceProducer class present
% in the EgoSampling code. It contains the template for the implementation  
% SemanticFastForward, which is responsible for the creation of the Semantic Fast-Forward video.
%
% $Date: July 26, 2017
% ________________________________________
classdef SemanticSequenceProducer < handle
    %SequenceExporter connects all the pieces together
    
    properties
        cfg; % ConfigWrapper
        sd;  % SequenceData
        SemanticData; %SemanticData
        Frame_indices; %Output frame indices
        FOE_locations; %Output FOE locations
        Instability_array; %Array of instability of the video
        Output_video_filename;
        TopSemanticValue; %The maximum Semantic Value Possible for the experiment
        use_range;
        optimize;
        store_results; %If enabled, it store the results in the general results file
    end
    
    methods
        function obj = SemanticSequenceProducer(sd, cfg, SemanticData, use_range, optimize)
            obj.cfg = cfg;
            obj.sd = sd;
            obj.Frame_indices = [];
            obj.FOE_locations = [];
            obj.Instability_array = [];
            obj.TopSemanticValue = 0;
            obj.use_range = use_range;
            obj.optimize = optimize;
            obj.store_results = ~optimize;
            obj.Output_video_filename = '';
            obj.SemanticData = SemanticData;
        end
                
        function run(obj)            
            obj.PreProcess();
            
            obj.PrepareCostTerms();
            
            if obj.optimize
                obj.GetTopSemanticValue();
                obj.OptimizeViaPSO();
                obj.store_results = 1;
            end
            
            if obj.use_range
                obj.SolveSemantic();
            else
                obj.Solve();
            end
            
            obj.PostProcess();
            
            if obj.cfg.get('SkipVideoOutput')~=1
                obj.ExportVideo();
            else
                fprintf('%Skipping Video output (''SkipVideoOutput''==%d)\n',log_line_prefix, obj.cfg.get('SkipVideoOutput'));
            end        
        end

        
        function PreProcess(obj)
        end
        
        function PrepareCostTerms(obj)
        end
        
        function GetTopSemanticValue(obj)
        end
        
        function OptimizeViaPSO(obj)
        end
        
        function EvalParticle(obj)
        end
        
        function SolveSemantic(obj)
        end
        
        function Solve(obj)
        end
        
        function GetResults(obj)
        end
        
        function StoreResults(obj)
        end
        
        function PostProcess(obj)
        end
                
        function ExportVideo(obj)
            vss = SemanticVideoSubSampler (obj.sd, obj.cfg, obj.SemanticData);
            fprintf('%sWriting %d frames to output video: %s...\n',log_line_prefix, length(obj.Frame_indices), obj.cfg.get('outputVideoFileName'));
            vss.subSampleVideo(obj.Frame_indices, obj.FOE_locations, obj.Instability_array, obj.cfg.get('outputVideoFileName'));
        end
        
    end
    
end


