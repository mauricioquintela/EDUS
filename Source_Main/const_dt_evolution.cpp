// time loop with constant dt
#pragma omp master
{
    it= 0;//iti - 10;
    time_loop = -0.21 / time_au_fs;// it*dt;
    itfi = (t_fin - time_loop)/dt;
}

/*
// I WROTE THIS vv //

if (rank_ == 0){
    #pragma omp barrier
    #pragma omp master
    {
        start = propagation_time::now();
        cout << "\nTIME PROPAGATION CLOCK STARTS NOW\n\n";
        duration = std::chrono::duration_cast<std::chrono::seconds>(start - start);
    }
}

// I WROTE THIS ^^ //
*/

#pragma omp barrier // sinchronise threads

for(int it_pr=0; it_pr<=itfi; it_pr++)
{   
    #pragma omp master
    { // shared variable, only master change it
        it = it_pr;
    }

    #pragma omp barrier

    if (it % it_resolution == 0){   
        #include "if_resolution.cpp" // printig all we need
    } // end if it_resolution
    
    if(Diff_Eq.Taylor){
        #include "Taylor_DE_solver_deriv_Polynom.cpp"
    } else if(Diff_Eq.Adams_Bashforth){
        #include "Linear_Multistep_ODE_solvers.cpp"
    } else{
        // RK time evolution
        #include "RungeKutta.cpp"
    }

    // I WROTE THIS vv //
//    #pragma omp barrier // sinchronise threads
//
//    // PRINTING POPULATION AT IT = IT_STEP_INT
//    double it_step = Laser_pumps[0].Period/(8*dt);
//    //cout << "\nPERIOD IS " << Laser_pumps[0].Period << "\n\n";
//    int it_step_int = (int)it_step;
//    //cout << "\nit_step_int " << it_step_int << "\n\n";
//    int itf = (Laser_pumps[0].tf-Laser_pumps[0].t0)/dt;
//    //cout << "\ntf and itf " << Laser_pumps[0].tf << " and " << itf << "\n\n";
//    if (it%it_step_int == 0 && it <= itf && print_kresolved_population){
//        std::string population_file_name = "population_it" + std::to_string(it) + ".txt";
//        if (rank_ == 0 && omp_get_thread_num() == 0) cout << "\nPRINTING POPULATION IN ITERATION " << it << "\n\n";
//      #pragma omp master
//        {
//            orderly_printing_population(population_file_name, rank_, nk, Ncv, P0, Unitary);
//        }
//
//    }
    #pragma omp barrier // sinchronise threads

    // PRINTING POPULATION AT IT = IT_STEP_INT
    double it_step = Laser_pumps[0].Period/(10*dt);
    //cout << "\nPERIOD IS " << Laser_pumps[0].Period << "\n\n";
    int it_step_int = (int)it_step;
    //cout << "\nit_step_int " << it_step_int << "\n\n";
    int itf = (Laser_pumps[0].tf-Laser_pumps[0].t0)/dt;
    int itfds = (int)(0.21/dt*time_au_fs);
    //cout << "\ntf and itf " << Laser_pumps[0].tf << " and " << itf << "\n\n";

    // SILLY TEST FOR MAURICIO TO EVOLVE AFTER PULSE IS OVER. I.e., exciton valley migration
    int mauricio_step = (int)(0.5/(dt*time_au_fs));
    if ((it-itfds)%it_step_int == 0 && it <= itf && (print_kresolved_population || print_kresolved_population_eigenbasis)){
        std::string population_file_name = "population_it" + std::to_string(it) + ".txt";
        if (rank_ == 0 && omp_get_thread_num() == 0) cout << "\nPRINTING POPULATION IN ITERATION " << it << "\n\n";
        #pragma omp master
        {
            if (print_kresolved_population) orderly_printing_population(population_file_name, rank_, nk, Ncv, P0, Unitary,"wannier");
            if (print_kresolved_population_eigenbasis) orderly_printing_population(population_file_name, rank_, nk, Ncv, P0, Unitary, "eigenbasis");
        }
    }
    else if (it%mauricio_step == 0)
    {
        std::string population_file_name = "population_it" + std::to_string(it) + ".txt";
        if (rank_ == 0 && omp_get_thread_num() == 0) cout << "\nPRINTING POPULATION IN ITERATION " << it << "\n\n";
        #pragma omp master
        {
            if (print_kresolved_population) orderly_printing_population(population_file_name, rank_, nk, Ncv, P0, Unitary,"wannier");
            if (print_kresolved_population_eigenbasis) orderly_printing_population(population_file_name, rank_, nk, Ncv, P0, Unitary, "eigenbasis");
        }
    }
    // I WROTE THIS ^^ //

    #pragma omp barrier // sinchronise threads
    #pragma omp master
    {
        if (it % (10*it_resolution) == 0){
            P_cond_max = P_cond_max_loc_mpi;
            MPI_Reduce(&P_cond_max_loc_mpi, &P_cond_max, 1, MPI_DOUBLE, MPI_MAX, root_rank, MPI_COMM_WORLD);    
            MPI_Bcast(& P_cond_max, 1, MPI_DOUBLE, root_rank, MPI_COMM_WORLD);
            if (rank_ == root_rank){
                cout <<  " P_cond_max " << P_cond_max << endl;
                cout <<  " n_cond " << n_cond << endl;
                if (OMP_private.Pk_min < -1e-13)  cout << "Negative population, P_min = " << OMP_private.Pk_min << endl;

                /*
                // I WROTE THIS vv //
                auto end = propagation_time::now();
                double last_duration = duration.count();
                duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
                double estimated_duration = (duration.count() - last_duration)*t_fin/(10*it_resolution*dt*3600);
                cout << "\n" << duration.count() << " SECONDS HAVE PASSED SINCE TIME PROPAGATION STARTED. ESTIMATING FROM THIS STEP IT SHOULD TAKE " << estimated_duration << " HOURS\n\n";
                // I WROTE THIS ^^ //
                */
            }
        }


        time_loop += dt;

        it += 1;

    }


    

    #pragma omp barrier // sinchronise threads
    

}//end time evolution
