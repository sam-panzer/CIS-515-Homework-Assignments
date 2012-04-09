function [ output_args ] = invert( M )

[nrows ncolumns] = size(M);

if nrows ~= ncolumns || det(M) == 0
    output_args = 'not invertable';
else
    N = crref([M eye(nrows)]);
    output_args = N(:,[nrows+1:2*nrows]);

end

