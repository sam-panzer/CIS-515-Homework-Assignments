function [ output_args ] = crref( M )

[nrows ncolumns] = size(M);
rank=0; %keeps track of # of liearly independent columns

for n=1:ncolumns
    %g = geomean(abs(M(:,n)));
    %closest = 0;
    %distance = Inf;
    %for m=n:nrows 
    %    if abs(abs(M(m,n)) - g) < distance
    %        closest = m;
    %        distance = abs(abs(M(m,n)) - g);
    %    end
    %end
    
    q = rank+1;
    while M(q,n) == 0 %find a nonzero pivot
        if q ~= nrows
            q = q + 1;
        else %entire column is zero, move on to next column
            q=0;
            break;
        end
    end

    if q ~= 0
        rank = rank + 1;
        E = eye(nrows);
        E(rank,rank)=0;
        E(q,q)=0;
        E(q,rank)=1;
        E(rank,q) = 1 / M(q,n); %set the pivot element to 1
        M = E * M;

        for m=1:nrows
            if m ~= rank
                E=eye(nrows);
                E(m,rank) = -M(m,n);
                M = E * M;
            end
        end
       
    end
    
    if rank == nrows
        break;
    end
    
    
end

output_args = M;

end

