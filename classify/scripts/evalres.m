#!/usr/bin/octave -qf

arg_list = argv();

[name, posterior, class] = textread(arg_list{1}, '%s %f %f');
thresholds = unique(sort([0; .5; 1; posterior]));
total = length(class);
p = sum(class == 1);
n = sum(class == 0);
tp = zeros(length(thresholds), 1);
fp = zeros(length(thresholds), 1);
for i = 1 : length(thresholds)
    tp(i) = sum((class == 1) & (posterior >= thresholds(i)));
    fp(i) = sum((class == 0) & (posterior >= thresholds(i)));
endfor

tpr = tp / p;
fpr = fp / n;
prec = tp ./ (tp + fp);
fmea = 2.0 ./ (prec.^-1 + tpr.^-1);
tn = n - fp;
fn = p - tp;
pa = (tp + tn) / total;
pe = p / total * (tp + fp) / total + n / total * (tn + fn) / total;
kappa = (pa - pe) ./ (1 - pe);

auc = 0;
for i = 2 : length(thresholds)
  auc += tpr(i) * (fpr(i - 1) - fpr(i));
endfor

fdisp(stdout, [thresholds, tpr, fpr, prec, fmea, kappa]);
fdisp(stdout, auc);

