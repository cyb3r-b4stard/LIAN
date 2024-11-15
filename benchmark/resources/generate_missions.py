import os
import pathlib

HEURISTIC_WEIGHT                  = 2
ANGLE_LIMIT                       = 25
DISTANCE                          = 15
DISTANCE_MIN                      = 2
DECREASE_DISTANCE_FACTOR          = 2
PIVOT_CIRCLE_RADIUS               = 0
STEP_LIMIT                        = -1
POST_SMOOTHING                    = 'false'
CURVATURE_HEURISTIC_WEIGHT        = 0
NUM_OF_PARENTS_TO_INCREASE_RADIUS = 2

CELLSIZE     = 24

LOG_LEVEL    = 2
LOG_PATH     = ' '
LOG_FILENAME = ' '

traversable  = {'.': '0', 'G': '0', '@': '1', 'O': '1', 'T': '1', 'S': '0', 'W': '1'}

abs_path       = pathlib.Path(__file__).parent.resolve().__str__()
path_maps      = abs_path + '/maps'
path_scenarios = abs_path + '/scenarios'

def generate_missions(n_tasks):
    for dir_scenarios in os.listdir(path_scenarios):
        for filename in os.listdir(f'{path_scenarios}/{dir_scenarios}'):
            with open(f'{path_scenarios}/{dir_scenarios}/{filename}') as file:
                # tasks = file.readlines()[-(n_tasks + 1):-1]
                tasks = file.readlines()[1:n_tasks + 2]
                
                for task_n, task in enumerate(map(lambda x: x.split(), tasks)):
                    bucket, map_name = task[0:2]
                    map_width, map_height, start_x, start_y, finish_x, finish_y = map(int, task[2:-1])
                    optimal_length = float(task[-1])

                    generate_mission(f'{path_maps}/{dir_scenarios}/{map_name}', map_width, map_height, start_x, start_y, finish_x, finish_y, name=f'{map_name}_{task_n}')

def generate_mission(map_path, map_width, map_height, start_x, start_y, finish_x, finish_y, name=''):
    with open(map_path, 'r') as file:
        w_lines = ['<?xml version="1.0" encoding="UTF-8" ?>\n', 
                   '<root>\n', 
                   '    <map>\n',
                   f'        <width>{map_width}</width>\n',
                   f'        <height>{map_height}</height>\n',
                   f'        <cellsize>{CELLSIZE}</cellsize>\n',
                   f'        <startx>{start_x}</startx>\n',
                   f'        <starty>{start_y}</starty>\n',
                   f'        <finishx>{finish_x}</finishx>\n',
                   f'        <finishy>{finish_y}</finishy>\n',
                   '        <grid>\n']

        for line_n, line in enumerate(file.readlines()):
            if line_n < 4:
                continue
            else:
                new_line = ' '.join([traversable[x] for x in line.rstrip()])
                w_lines.append(f'            <row>{new_line}</row>\n')
        
        w_lines.extend(['        </grid>\n', 
                    '    </map>\n', 
                    '    <algorithm>\n',
                    f'        <hweight>{HEURISTIC_WEIGHT}</hweight>\n',
                    f'        <anglelimit>{ANGLE_LIMIT}</anglelimit>\n',
                    f'        <distance>{DISTANCE}</distance>\n',
                    f'        <distancemin>{DISTANCE_MIN}</distancemin>\n',
                    f'        <decreaseDistanceFactor>{DECREASE_DISTANCE_FACTOR}</decreaseDistanceFactor>\n',
                    f'        <pivotCircleRadius>{PIVOT_CIRCLE_RADIUS}</pivotCircleRadius>\n',
                    f'        <steplimit>{STEP_LIMIT}</steplimit>\n',
                    f'        <postsmoothing>{POST_SMOOTHING}</postsmoothing>\n',
                    f'        <curvatureHeuristicWeight>{CURVATURE_HEURISTIC_WEIGHT}</curvatureHeuristicWeight>\n',
                    f'        <numOfParentsToIncreaseRadius>{NUM_OF_PARENTS_TO_INCREASE_RADIUS}</numOfParentsToIncreaseRadius>\n',
                    '    </algorithm>\n',
                    '    <options>\n',
                    f'        <loglevel>{LOG_LEVEL}</loglevel>\n',
                    f'        <logpath{LOG_PATH}/>\n',
                    f'        <logfilename{LOG_FILENAME}/>\n',
                    '    </options>\n',
                    '</root>'])
        if name == '': 
            name = map_path.split('/')[-1]

    with open(f'{path_maps}/xml/{name}.xml', 'w') as w_file:
        w_file.writelines(w_lines)
    

if __name__ == '__main__':
    generate_missions(n_tasks=20)