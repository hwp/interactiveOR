#!/usr/bin/octave -qf

arg_list = argv();

[name, posterior, class] = textread(arg_list{1}, '%s %f %f');
thresholds = [unique(sort([[0:.1:1]'; posterior])); 1];
total = length(class);
p = sum(class == 1);
n = sum(class == 0);
tp = zeros(length(thresholds), 1);
fp = zeros(length(thresholds), 1);
for i = 1 : length(thresholds)
  if i < length(thresholds)
    tp(i) = sum((class == 1) & (posterior >= thresholds(i)));
    fp(i) = sum((class == 0) & (posterior >= thresholds(i)));
  else
    tp(i) = 0;
    fp(i) = 0;
  endif
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
  auc += (tpr(i) + tpr(i - 1)) * (fpr(i - 1) - fpr(i)) / 2.0;
endfor

disp([thresholds, tpr, fpr, prec, fmea, kappa]);
printf('%.3f\n', auc);

