function y = ecf_draw_graph(filename)

D = load(filename);
t = D(:,1);
U = D(:,2);

plot(t,U);

endfunction
