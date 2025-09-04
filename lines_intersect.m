function intersection = lines_intersect(p1, dir1, p2, dir2)
%% Inicializáció   
    arguments(Input)
        p1 (2,1) double 
        dir1 (2,1) double
        p2 (2,1) double
        dir2 (2,1) double
    end
    arguments(Output)
        intersection (2, 1) double
    end
    line1 = zeros(1,3);
    line2 = zeros(1,3);
    intersection = zeros(2,1);
%% Kód    
%LINE_INTERSECT Summary of this function goes here
%   Detailed explanation goes here
    
    % source: https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection#Using_homogeneous_coordinates
    
    % normal vectors
    n1 = [dir1(2), -dir1(1)];
    n2 = [dir2(2), -dir2(1)];

    
    % line description with a*x + b*y + c = 0
    % (n_1*x + n_2*y = n_1*x0 + n_2*y0
    line1 = [n1(1), n1(2), -dot(p1, n1)];
    line2 = [n2(1), n2(2), -dot(p2, n2)];
    
    % intersection in homogeneous coordinates
    intersection_hom = cross(line1, line2);
    
    % convert the homogeneous coordinates to cartesian
%% Kimenet    
    if(abs(intersection_hom(3)) < 1e-6)
        intersection = [NaN, NaN];
    else
        intersection = [intersection_hom(1)/intersection_hom(3), intersection_hom(2)/intersection_hom(3)];
    end
end

