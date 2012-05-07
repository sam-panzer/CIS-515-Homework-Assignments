function [ E ] = gramschmidt( W )

%W is input matrix whose rows are the vectors to be orthonormalized
%E is output matrix whose rows are the orthonormalized vectors of W

dimensions = size(W);

V = zeros(dimensions(1), dimensions(2));
E = zeros(dimensions(1), dimensions(2));

for i=1:dimensions(1)
    V(i,:) = W(i,:);
    for j=1:i-1
        
        V(i,:) = V(i,:) - dot(V(i,:),V(j,:)) / dot(V(j,:),V(j,:)) * V(j,:);
        
    end
    
    V(i,:) = V(i,:);
    
    E(i,:) = V(i,:) / norm(V(i,:));
    
end