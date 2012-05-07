function [ V ] = gramschmidt( W )

%W is input matrix whose rows are the vectors to be orthonormalized
%V is output matrix whose rows are the orthonormalized vectors of W

dimensions = size(W);

V = zeros(dimensions(1), dimensions(2));

for i=1:dimensions(1)
    V(i,:) = W(i,:) / norm(W(i,:));
    for j=1:i-1
        
        V(i,:) = V(i,:) - dot(V(i,:),V(j,:)) / norm(V(i,:)) * V(j,:);
        
    end
    
end

