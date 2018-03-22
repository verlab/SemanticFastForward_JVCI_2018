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

function level = My_Otsu(histogramCounts, total, max_x)
%% OTSU automatic thresholding method
    sumB = 0;
    wB = 0;
    maximum = 0.0;
    sum1 = sum((0:max_x-1).*histogramCounts);
    for ii=1:max_x
        wB = wB + histogramCounts(ii);
        if (wB == 0)
            continue;
        end
        wF = total - wB;
        if (wF == 0)
            break;
        end
        sumB = sumB +  (ii-1) * histogramCounts(ii);
        mB = sumB / wB;
        mF = (sum1 - sumB) / wF;
        between = wB * wF * (mB - mF) * (mB - mF);
        if ( between >= maximum )
            level = ii;
            maximum = between;
        end
    end
end
