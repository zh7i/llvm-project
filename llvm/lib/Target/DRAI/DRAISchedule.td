def DRAISchedModel : SchedMachineModel {
  // The M88110 can issue 1 instructsions.
  let IssueWidth = 1;

  // In-order CPU.
  let MicroOpBufferSize = 0;

  //let LoadLatency = ?;
  //let MispredictPenalty = ?;

  // Not finished yet.
  let CompleteModel = 0;
  let NoModel = 1;
}