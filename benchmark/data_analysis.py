import pandas as pd
import matplotlib.pyplot as plt
import os
import pathlib

abs_path           = pathlib.Path(__file__).parent.resolve().__str__()
logs_basic_path    = abs_path + '/out/logs_basic'
logs_modified_path = abs_path + '/out/logs_modified'

dict_basic    = dict()
dict_modified = dict()

def extract_data(dict, line):
    if line.strip().startswith('<summary'):
        path_found, n_steps, n_nodes, n_sections, length, length_scaled, time, max_angle, accum_angle = line.split()[1:10]

        path_found    = int(path_found.split('=')[1].strip('"') == 'true')
        n_nodes       = int(n_nodes.split('=')[1].strip('"'))
        n_steps       = int(n_steps.split('=')[1].strip('"'))
        n_sections    = int(n_sections.split('=')[1].strip('"'))
        length        = float(length.split('=')[1].strip('"'))
        length_scaled = float(length_scaled.split('=')[1].strip('"'))
        time          = float(time.split('=')[1].strip('"'))
        max_angle     = float(max_angle.split('=')[1].strip('"'))
        accum_angle   = float(accum_angle.split('=')[1].strip('"'))

        if not path_found:
            n_nodes = 0
            n_steps = 0
            n_sections = 0
            length = 0
            length_scaled = 0
            time = 0
            max_angle = 0
            accum_angle = 0
            
        dict[filename] = {
            'path_found': path_found,
            'n_nodes': n_nodes,
            'n_steps': n_steps,
            'n_sections': n_sections,
            'length': length,
            'length_scaled': length_scaled,
            'time': time,
            'max_angle': max_angle,
            'accum_angle': accum_angle
        }

for filename in os.listdir(logs_basic_path):
    with open(f'{logs_basic_path}/{filename}', 'r') as file:
        for line in file.readlines():
            extract_data(dict_basic, line)

    with open(f'{logs_modified_path}/{filename}', 'r') as file:
        for line in file.readlines():
            extract_data(dict_modified, line)

data_basic    = pd.DataFrame(dict_basic).transpose()
data_modified = pd.DataFrame(dict_modified).transpose()


def calculate_ratio(first, second, parameter):
    tmp_first = first[first.index.isin(second.index)]
    
    return len(second[parameter] == tmp_first[parameter]) / len(first[first[parameter] > 0])
    

def calculate_mean(data, parameter):
    return data[parameter].mean()

fig, ax = plt.subplots(nrows=2, ncols=3, figsize=(12, 8))
width = 0.6

# Success Rate
basic_statistic    = calculate_mean(data_basic, 'path_found')
modified_statistic = calculate_mean(data_modified, 'path_found')

# ratio = calculate_ratio(data_basic, data_modified, 'path_found')
ratio = modified_statistic / basic_statistic

ax[0, 0].bar('basic-LIAN', 1, width)
ax[0, 0].bar('modified-LIAN', ratio, width)
ax[0, 0].set_title('Success Rate')

# Path Length
basic_statistic    = calculate_mean(data_basic, 'length')
modified_statistic = calculate_mean(data_modified, 'length')

# ratio = calculate_ratio(data_basic, data_modified, 'length')
ratio = modified_statistic / basic_statistic

ax[0, 1].bar('basic-LIAN', 1, width)
ax[0, 1].bar('modified-LIAN', ratio, width)
ax[0, 1].set_title('Path Length')

# Sections
basic_statistic    = calculate_mean(data_basic, 'n_sections')
modified_statistic = calculate_mean(data_modified, 'n_sections')

# ratio = calculate_ratio(data_basic, data_modified, 'n_sections')
ratio = modified_statistic / basic_statistic

ax[0, 2].bar('basic-LIAN', 1, width)
ax[0, 2].bar('modified-LIAN', ratio, width)
ax[0, 2].set_title('Sections')

# Steps
basic_statistic    = calculate_mean(data_basic, 'n_steps')
modified_statistic = calculate_mean(data_modified, 'n_steps')

ax[1, 0].bar('basic-LIAN', 1, width)
ax[1, 0].bar('modified-LIAN', modified_statistic / basic_statistic, width)
ax[1, 0].set_title('Steps')

# Time
basic_statistic    = calculate_mean(data_basic, 'time')
modified_statistic = calculate_mean(data_modified, 'time')

ax[1, 1].bar('basic-LIAN', 1, width)
ax[1, 1].bar('modified-LIAN', modified_statistic / basic_statistic, width)
ax[1, 1].set_title('Time')

# Memory
basic_statistic    = calculate_mean(data_basic, 'n_nodes')
modified_statistic = calculate_mean(data_modified, 'n_nodes')

ax[1, 2].bar('basic-LIAN', 1, width)
ax[1, 2].bar('modified-LIAN', modified_statistic / basic_statistic, width)
ax[1, 2].set_title('Memory')

plt.tight_layout()
plt.savefig("results.pdf")