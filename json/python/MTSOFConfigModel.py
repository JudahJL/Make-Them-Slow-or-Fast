import argparse
import json
import sys
from pathlib import Path
from typing import Literal, Annotated

from pydantic import BaseModel, ConfigDict, Field


class LogLevelModel(BaseModel):
    model_config = ConfigDict(extra="forbid")
    LogLevel: Literal["trace", "debug", "info", "warning", "error", "critical", "off"]


type EnableType = bool
type GravityType = float
type SpeedType = float
type MinType = float
type MaxType = float


class ChangeGravityModel(BaseModel):
    model_config = ConfigDict(extra="forbid")
    Enable: EnableType
    Gravity: GravityType


class ChangeSpeedModel(BaseModel):
    model_config = ConfigDict(extra="forbid")
    Enable: EnableType
    Speed: SpeedType


class LimitSpeedModel(BaseModel):
    model_config = ConfigDict(extra="forbid")
    Enable: EnableType
    Min: MinType
    Max: MaxType


class LimitGravityModel(BaseModel):
    model_config = ConfigDict(extra="forbid")
    Enable: EnableType
    Min: MinType
    Max: MaxType


class RandomizeSpeedModel(BaseModel):
    model_config = ConfigDict(extra="forbid")
    Enable: EnableType
    Min: MinType
    Max: MaxType


class AimedModel(BaseModel):
    class FireAndForgetModel(BaseModel):
        model_config = ConfigDict(extra="forbid")
        ChangeSpeed: Annotated[ChangeSpeedModel, Field(alias="Change Speed")]
        LimitSpeed: Annotated[LimitSpeedModel, Field(alias="Limit Speed")]
        RandomizeSpeed: Annotated[RandomizeSpeedModel, Field(alias="Randomize Speed")]
        ChangeGravity: Annotated[ChangeGravityModel, Field(alias="Change Gravity")]
        LimitGravity: Annotated[LimitGravityModel, Field(alias="Limit Gravity")]

    model_config = ConfigDict(extra="forbid")

    Enable: EnableType
    FireAndForget: Annotated[FireAndForgetModel, Field(alias="Fire and Forget")]


class MTSOFModel(BaseModel):
    model_config = ConfigDict(populate_by_name=True, extra="forbid", strict=True, validate_assignment=True,
                              validate_default=True, regex_engine='python-re')
    Logging: LogLevelModel
    Aimed: AimedModel


def main(using_rapidjson: bool):
    base_dir = Path(__file__).parent.parent.parent.resolve()
    skse_plugins_dir = base_dir / 'contrib' / 'Distribution' / 'data' / 'skse' / 'plugins'
    presets_dir = skse_plugins_dir / 'Make Them Slow or Fast' / 'presets'

    for file in presets_dir.glob('*.json'):
        if file.exists():
            with open(file, 'r') as f:
                try:
                    MTSOFModel.model_validate_json(strict=True, json_data=f.read())
                except Exception as e:
                    print(type(e).__name__, str(e))

    schema = skse_plugins_dir / "MTSOFConfig_schema.json"

    with open(schema, 'w') as f:
        # noinspection PyRedundantParentheses
        if (using_rapidjson is not None) and (using_rapidjson is True):  # rapidjson uses draft 4, so a workaround
            temp = MTSOFModel.model_json_schema(ref_template="#/definitions/{model}")
            temp["definitions"] = temp.pop("$defs")
            # noinspection PyTypeChecker
            json.dump(obj=temp, indent=2, fp=f)
        else:
            # noinspection PyTypeChecker
            json.dump(obj=MTSOFModel.model_json_schema(), indent=2, fp=f)  # this creates a draft 2020-12 schema


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Toggle between using rapidjson(draft 4) or draft 2020-12.")
    parser.add_argument('--rapidjson', action=argparse.BooleanOptionalAction)

    args = parser.parse_args()

    main(args.rapidjson)
