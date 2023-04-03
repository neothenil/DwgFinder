import typing as t
import subprocess


class Reader:
    def __init__(self, exe_path: str, args: t.Optional[t.List[str]] = None) -> None:
        self.exe_path = exe_path
        self.args = args
        self.result = (False, "Not executed yet")

    def execute(self, timeout: t.Union[int, float] = 120) -> None:
        args = [self.exe_path]
        if self.args is not None:
            args.extend(self.args)
        try:
            complete_process = subprocess.run(
                args, capture_output=True, timeout=timeout
            )
        except subprocess.TimeoutExpired:
            self.result = (False, "Reading dwg/dxf timeout")
            return
        if complete_process.returncode == 0:
            self.result = (True, "")
            return
        error_message = str(complete_process.stderr, encoding="UTF16").strip()
        self.result = (False, error_message)

    def check_result(self) -> t.Tuple[bool, str]:
        return self.result
