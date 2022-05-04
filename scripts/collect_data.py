def collect_data(time_series: SortedDict,id,new_data):
    if len(time_series) == 0:
        time_series.update({t:{id:s} for t,s in new_data})
    return None
    for t,y in new_data:
        data = {id:y}
        if t in time_series:
            data = align_merge(time_series[t],data)
        time_series[t] = data

def test_collect_data_ideal():
    time_series = SortedDict()
    collect_data(time_series,1,[[0,1],[1,2]])
    assert time_series == {0:{1:1},1:{1:2}}
    collect_data(time_series,2,[[0,3],[1,4]])
    assert time_series == {0:{1:1,2:3},1:{1:2,2:4}}

def test_collect_data_noisy_easy():
    time_series = SortedDict()
    collect_data(time_series,1,[[1,1],[11,2]])
    assert time_series == {1:{1:1},11:{1:2}}
    collect_data(time_series,2,[[0,3],[12,4]])
    assert time_series == {0.5:{1:1,2:3},11.5:{1:2,2:4}}
