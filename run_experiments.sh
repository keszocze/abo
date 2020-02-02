rm -r result
mkdir result
cd benchmark
echo "Computation time for error metric steps (table 1)"
./benchmark_ace_steps &> ../result/table_1_ace_steps
echo "Computation time for average case error computation (table 3)"
./benchmark_average_case_error &> ../result/table_3_ace_time
echo "Computation time for average case relative error approximation (table 4)"
./benchmark_acr_approx &> ../result/table_4_acr_approx
cd ../experiments
echo "Running bucket minimization of ISCAS 85 functions (table 5)"
./minimize_iscas &> ../result/table_5_bucket_iscas
echo "Running NSGA2 minimization of ISCAS 85 functions (table 5)"
./nsga2_iscas &> ../result/table_5_nsga2_iscas
echo "Running bucket minimization of adders (table 6)"
./minimize_adders &> ../result/table_6_bucket_adders
echo "Running bucket minimization of adders (table 6)"
./nsga2_adders &> ../result/table_6_nsga2_adders
