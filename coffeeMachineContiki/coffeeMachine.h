/*******************************************************************************
*
* Defitinion of states
*
*
*******************************************************************************/

typedef enum{
  NOCOIN = 0,
  NICKEL = 5, 
  DIME = 10
} COINS;




/***( EFSM states )************************************************************/

typedef enum
{
  Idle,
  PaidTen,
  PaidFive,
  MakingCoffeeOrTea,
  WaterForCoffee,
  Warming
} PCONTROLLER_STATES;                            /* EFSM states for process controller */

typedef enum
{
  IdleH
  WarmingH
} PHARDWARE_STATES;                            /* EFSM states for process hardware */

typedef enum
{
  IdleT
} PTIMER_STATES;                            /* EFSM states for process timer */

