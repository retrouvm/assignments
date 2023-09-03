#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Oct 19 16:07:07 2020

@author: luki
"""
import numpy as np
import matplotlib.pyplot as plt
from sklearn import linear_model
import pdb

experience = np.array([2,3,5, 13, 8])
salary = np.array([15, 28, 42, 64, 50])

#x
exp_mean = np.mean(experience)
print("The mean of experience (x) is ", exp_mean)
#y
salary_mean = np.mean(salary)
print("The mean of salary (y) is ", salary_mean)

n = (experience - exp_mean).dot(salary - salary_mean)
d = (experience - exp_mean).dot(experience - exp_mean)

beta = n/d
alpha = salary_mean - beta*exp_mean

print("Slope beta= ", beta)
print("Intercept alpha= ", alpha)



print("Calculated by sklearn...")
model = linear_model.LinearRegression()
model.fit(experience.reshape(-1,1),salary.reshape(-1,1))
print('Slope β =', model.coef_[0][0])
print('Intercept α =', model.intercept_[0])
