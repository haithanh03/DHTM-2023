paths = [
    ["./xeTai_Chan1", "xeTai_Chan1"],
    ["./xeTai_Chan2", "xeTai_Chan2"],
    ["./xeTai_Cua", "xeTai_Cua"],
    ["./xeTai_Dau", "xeTai_Dau"],
    ["./xeTai_DenSau", "xeTai_DenSau"],
    ["./xeTai_GatNuoc", "xeTai_GatNuoc"],
    ["./xeTai_Guong", "xeTai_Guong"],
    ["./xeTai_Kinh", "xeTai_Kinh"],
    ["./xeTai_NoiThat1", "xeTai_NoiThat1"],
    ["./xeTai_TayNam", "xeTai_TayNam"],
    ["./xeTai_TrucGuong1", "xeTai_TrucGuong1"],
    ["./xeTai_TrucGuong2", "xeTai_TrucGuong2"],
    ["./matTroi", "matTroi"],
    ["./Battery", "battery"],
    ["./Than_Battery", "than_battery"],
    ["./house","house"],
    ["./cuachinh", "cuachinh"],
    ["./tayNamCuaChinh", "tayNamCuaChinh"],
    ["./cuaSo1", "cuaSo1"],
    ["./caighe", "caighe"],
    ["./khungfridge", "khungfridge"],
    ["./ngantulanh1", "ngantulanh1"],
    ["./ngantulanh2", "ngantulanh2"],
    ["./canhtulanh1", "canhtulanh1"],
    ["./canhtulanh2", "canhtulanh2"],
    ["./fan", "fan"],
    ["./bed", "bed"],
]

f_main = open("./all_models.h", "w")

for my_index in range(0, len(paths)):
    folder_path = paths[my_index][0]
    model_name = paths[my_index][1]

    f_main.write(f'#include "{folder_path}/{model_name}.h"\n')

    variable_name = folder_path.replace(".", "").replace("/", "")

    vertices_output = f"vec4 {variable_name.lower()}_vertices[] = " + "{"
    normal_output = f"vec3 {variable_name.lower()}_normals[] = " + "{"
    ambient_output = f"vec4 {variable_name.lower()}_ambients[] = " + "{"
    diffuse_output = f"vec4 {variable_name.lower()}_diffuses[] = " + "{"
    specular_output = f"vec4 {variable_name.lower()}_speculars[] = " + "{"
    

    point_path = f"{folder_path}/{model_name}.obj"
    material_path = f"{folder_path}/{model_name}.mtl"

    material_text = open(material_path, 'r').readlines()
    point_text = open(point_path, 'r').readlines()

    material_list = {}

    m_name = None
    for i in material_text:
        i = i.replace('\n', '')
        if (i.find("newmtl") != -1):
            m_name = i[7:]
            material_list[m_name] = {}
        if (m_name != None):
            if (i.find("Ka") != -1):
                material_list[m_name]["Ka"] = i[3:]
            if (i.find("Kd") != -1):
                material_list[m_name]["Kd"] = i[3:]
            if (i.find("Ks") != -1):
                material_list[m_name]["Ks"] = i[3:]

    vertices = []
    normal = []
    current_material = None
    count = 0

    for i in point_text:
        i = i.replace("\n", "")

        if (i[:6] == "usemtl"):
            current_material = i[7:]

        if (i[:2] == "v "):
            vertices.append(i[2:])
        if (i[:2] == "vn"):
            normal.append(i[3:])
        if (i[:2] == "f "):
            map_list = i.split()[1:]

            for m in map_list:
                m_list = m.split("/")

                vertices_output += "vec4(" + vertices[(int)(
                    m_list[0]) - 1].replace(" ", "f,") + ", 1.0f),\n"

                normal_output += "vec3(" + normal[(int)
                                                  (m_list[2]) - 1].replace(" ", "f,") + "),\n"

                ambient_output += "vec4(" + material_list[current_material]["Ka"].replace(
                    " ", "f,") + ", 1.0f),\n"

                diffuse_output += "vec4(" + material_list[current_material]["Kd"].replace(
                    " ", "f,") + ", 1.0f),\n"

                specular_output += "vec4(" + material_list[current_material]["Ks"].replace(
                    " ", "f,") + ", 1.0f),\n"

                count += 1

    vertices_output += "};"
    normal_output += "};"
    ambient_output += "};"
    diffuse_output += "};"
    specular_output += "};"

    size_out = f"const int {variable_name.lower()}_size = {count};"

    file_output = open(folder_path + f"/{model_name}.h", "w")

    file_output.write(
        f"{size_out}\n{vertices_output}\n{normal_output}\n{ambient_output}\n{diffuse_output}\n{specular_output}"
    )
    file_output.close()

f_main.close()

print("done")
