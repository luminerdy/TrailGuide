"""Contained preparation paths. Assumes locally owned, non-concurrent trees."""
from pathlib import Path
from contextlib import contextmanager
import shutil
import tempfile


@contextmanager
def preparation_stage(private):
    """Use one same-filesystem stage; retain backups if rollback itself fails."""
    stage = Path(tempfile.mkdtemp(prefix='.preparation-', dir=private))
    try:
        yield stage
    finally:
        if not any((stage / name).exists() for name in ('.retain-backup', '.rollback-needed')):
            shutil.rmtree(stage)


def publish(stage, private, sketch_name):
    """Manifest-last commit with rollback, not three allegedly atomic renames.

    No concurrent readers/writers or power-loss recovery is promised. During the
    short publish interval no completion manifest exists. A failed rollback
    retains its backup tree for manual recovery rather than deleting evidence.
    """
    backup = stage / '.previous'
    backup.mkdir()
    # Create the retention guard BEFORE moving any previous output. Recovery
    # reporting can fail independently; only successful commit/rollback clears it.
    guard = stage / '.retain-backup'
    guard.touch()
    relatives = ('preparation.json', 'prepared/' + sketch_name, 'libraries', 'config')
    saved, installed = [], []
    try:
        # Withhold the old completion record before changing any described data.
        for relative in relatives:
            target = checked_path(private / relative, private)
            if target.exists():
                previous = backup / relative
                previous.parent.mkdir(parents=True, exist_ok=True)
                target.replace(previous)
                saved.append(relative)
            if relative != 'preparation.json':
                target.parent.mkdir(parents=True, exist_ok=True)
                checked_path(stage / relative, stage).replace(target)
                installed.append(relative)
        (stage / 'preparation.json').replace(private / 'preparation.json')
        installed.append('preparation.json')
        guard.unlink()
    except BaseException as error:
        try:
            for relative in reversed(installed):
                target = checked_path(private / relative, private)
                if target.is_dir():
                    shutil.rmtree(target)
                else:
                    target.unlink()
            # The original manifest is restored last, after all old data.
            for relative in reversed(saved):
                (backup / relative).replace(checked_path(private / relative, private))
        except BaseException as rollback_error:
            error.add_note('Rollback incomplete: ' + repr(rollback_error) + '; preserve recovery tree: ' + str(stage))
            try:
                (stage / '.rollback-needed').write_text(str(rollback_error) + '\n')
            except BaseException as marker_error:
                error.add_note('Rollback marker write failed: ' + repr(marker_error))
        else:
            try:
                guard.unlink()
            except BaseException as guard_error:
                error.add_note('Rollback complete but retention guard cleanup failed: ' + repr(guard_error))
        raise


def checked_path(path, anchor):
    """Return a lexical absolute child; reject every existing symlink ancestor."""
    try:
        path, anchor = Path(path), Path(anchor)
    except (TypeError, ValueError) as error:
        raise ValueError('Preparation paths must be filesystem paths') from error
    if '..' in path.parts or '..' in anchor.parts:
        raise ValueError('Preparation paths cannot contain parent traversal')
    path, anchor = path.absolute(), anchor.absolute()
    if path != anchor and anchor not in path.parents:
        raise ValueError('Output must be inside owned preparation root')
    for candidate in (path, *path.parents):
        if candidate.is_symlink():
            raise ValueError('Preparation path contains a symlink: ' + str(candidate))
        if candidate != path and candidate.exists() and not candidate.is_dir():
            raise ValueError('Preparation ancestor is not a directory: ' + str(candidate))
    return path


def validate_output(root, output, anchor_relative, sketch_name):
    """Validate independent canonical anchor and all replaceable destinations."""
    anchor = Path(root) / anchor_relative
    checked_path(anchor, anchor)
    private = checked_path(output, anchor)
    for relative in ('prepared', 'prepared/' + sketch_name, 'libraries', 'config', 'preparation.json'):
        target = checked_path(private / relative, anchor)
        if target.exists() and (not target.is_file() if relative == 'preparation.json' else not target.is_dir()):
            raise ValueError('Unsupported preparation destination type: ' + str(target))
        if target.is_dir():
            for child in target.rglob('*'):
                checked_path(child, anchor)
    return private
